#include <stdlib.h>
#include <assert.h>

#include "binaryfile.h"

#include "tokenizer.h"
#include "exitingalloc.h"
#include "files.h"
#include "command.h"

inline size_t binfile_size(const BinaryFile* file)
{
	return (sizeof(BinaryFile) + 
			(file->ncommands - 1) * sizeof(BinCommand));
}

CommandsContainer* CContainerInit() 
{
	CommandsContainer* retval = reinterpret_cast<CommandsContainer*>(
		exiting_malloc(sizeof(CommandsContainer))
	);
	
	retval->lsize = 0;
	retval->lcapacity = 32;
	retval->labels = reinterpret_cast<LabelEntry*>(
		exiting_malloc(sizeof(LabelEntry) * retval->lcapacity)
	);
	
	retval->ccapacity = 1;
	retval->file = reinterpret_cast<BinaryFile*>(
		exiting_malloc(sizeof(BinaryFile))
	);
	retval->file->ncommands = 0;
	
	return retval;
}

int CContainerReserve(CommandsContainer* container, size_t size)
{
	assert(container != nullptr);
	assert(size > 0);
	
	if (container->ccapacity == size)
		return 0;
	
	size_t bsize = sizeof(BinaryFile) + (size - 1) * sizeof(BinCommand);
	container->file = reinterpret_cast<BinaryFile*>(
		exiting_realloc(container->file, bsize)
	);
	
	container->ccapacity = size;
	
	return 0; //TODO ERROR
}

int CContainerAdd(CommandsContainer* container, BinCommand cmd)
{
	assert(container != nullptr);
	
	if (container->file->ncommands == container->ccapacity)
		CContainerReserve(container, container->ccapacity * 2);
		
	container->file->commands[container->file->ncommands++] = cmd;
	
	return 0; //TODO ERROR
}

int CContainerShrink(CommandsContainer* container)
{
	assert(container != nullptr);
	
	size_t size = container->file->ncommands;
	
	return CContainerReserve(container, size);
}

void CContainerDeInit(CommandsContainer* container)
{
	assert(container);
	
	free(container->labels);
	free(container);
}

inline void LabelsReserve(CommandsContainer* container)
{
	assert(container);
	
	if (container->lcapacity == container->lsize) {
		container->lcapacity *= 2;
		container->labels = reinterpret_cast<LabelEntry*>(
			exiting_realloc(container->labels, container->lcapacity)
		);
	}
}

inline int LabelsFind(CommandsContainer* container, const char* name)
{
	assert(container);
	assert(container->labels);
	assert(name);
	
	for (int i = 0; i < container->lsize; ++i)
		if (strcmp(container->labels[i].name, name) == 0) 
			return i;
	
	return -1;
}

int CContainerLabelSet(CommandsContainer* container, const char* name, int ncommand)
{
	assert(container);
	assert(container->labels);
	assert(name);
	
	int i = LabelsFind(container, name);
	if (i < 0) {
		LabelsReserve(container);
		
		container->labels[container->lsize++] = {name, ncommand};
		
		return 0;
	}
	
	if (container->labels[i].ncommand == -1) {
		container->labels[i].ncommand = ncommand;
		
		return 0;
	}
	
	return 1;
}

int CContainerLabelGet(CommandsContainer* container, const char* name)
{
	assert(container);
	assert(container->labels);
	assert(name);
	
	int i = LabelsFind(container, name);
	if (i < 0) {
		LabelsReserve(container);
		
		i = container->lsize++;
		
		container->labels[i] = {name, -1};
	}
	
	return i;
}

int CContainerPushLabels(CommandsContainer* container)
{
	for (int i = 0; i < container->lsize; ++i)
		if (container->labels[i].ncommand == -1)
			return 1;
	
	for (int i = 0; i < container->file->ncommands; ++i) {
		BinCommand* cmd = &container->file->commands[i];
		
		int id = get_command_id(cmd->type);
		if (id == get_command_id("JUMP") || 
			id == get_command_id("CALL"))
			cmd->arg2 = container->labels[cmd->arg2].ncommand;
	}
	
	return 0;		
}

int process_tokens(	const char **tokens, 
                    size_t ntokens, 
					size_t nline, 
                    void* arg)
{
    assert(tokens);
    assert(arg);
    
    for (size_t i = 0; i < ntokens; ++i)
		if (*(tokens[i]) == ';')
			ntokens = i;
    
	if (ntokens == 0) return 0;
		
	int id = get_command_id(tokens[0]);
	
	if (id < 0) {
		printf("## ERROR: Unknown command \"%s\" on line %lu\n", 
			tokens[0], nline);
		
		return 1;
	}
	
	CommandsContainer* container = 
		reinterpret_cast<CommandsContainer*>(arg);
		
	int error = get_processor(id)(tokens, ntokens, container);
	
	if (error) printf("Error on %s\n", tokens[0]);
	
	return error;
}

BinaryFile* BinaryFileFromVMFile(const char* fname)
{
    assert(fname);
    
    char* data = read_file_str(fname);
    
    CommandsContainer* container = CContainerInit();
	
	int error = tokenize_lines(	data, 
								" \t", 
								process_tokens, 
								container
								);
    
    if (error) {
		free(container->file);
		CContainerDeInit(container);
		free(data);
		
		return NULL;
	}
	
	error = CContainerPushLabels(container);
	
	if (error) {
		free(container->file);
		CContainerDeInit(container);
		free(data);
		
		return NULL;
	}
    
    CContainerShrink(container);
    
    BinaryFile* retval = container->file;
    
    free(data);                            
    CContainerDeInit(container);
    
    return retval;
}

BinaryFile* BinaryFileFromBinFile(const char* fname)
{
	assert(fname);
	
	FileData data = read_file_bin(fname);
    BinaryFile* retval = reinterpret_cast<BinaryFile*>(data.ptr);
	
    size_t cmds_size = (retval->ncommands - 1) * sizeof(BinCommand);
	size_t expected = sizeof(BinaryFile) + cmds_size;
				
	if (expected != data.size) {
		printf("## Error reading binary file: size doesn't match\n");
		
		retval = 0;
	}
	
	return retval;
}

int BinaryFileToFile(BinaryFile* file, const char* fname)
{
	assert(file);
	assert(fname);
	
	FILE* fp = exiting_fopen(fname, "w");
	
	size_t size = binfile_size(file);
	
	size_t written = fwrite(file, 1, size, fp);
	
	fclose(fp);
	
	return (written != size);
}

