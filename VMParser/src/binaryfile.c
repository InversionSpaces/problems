#include <stdlib.h>
#include <assert.h>

#include "binaryfile.h"

#include "tokenizer.h"
#include "exitingalloc.h"
#include "files.h"
#include "command.h"

CommandsContainer* CommandsContainerInit() 
{
	CommandsContainer* retval = reinterpret_cast<CommandsContainer*>(
		exiting_malloc(sizeof(CommandsContainer))
	);
	
	retval->capacity = 1;
	retval->file = reinterpret_cast<BinaryFile*>(
		exiting_malloc(sizeof(BinaryFile))
	);
	
	retval->file->ncommands = 0;
	
	return retval;
}

int CommandsContainerReserve(CommandsContainer* container, size_t size)
{
	assert(container != nullptr);
	assert(size > 0);
	
	if (container->capacity == size)
		return 0;
	
	size_t bsize = sizeof(BinaryFile) + (size - 1) * sizeof(BinCommand);
	container->file = reinterpret_cast<BinaryFile*>(
		exiting_realloc(container->file, bsize)
	);
	
	container->capacity = size;
	
	return 0; //TODO ERROR
}

int CommandsContainerAdd(CommandsContainer* container, BinCommand cmd)
{
	assert(container != nullptr);
	
	if (container->file->ncommands == container->capacity)
		CommandsContainerReserve(container, container->capacity * 2);
		
	container->file->commands[container->file->ncommands++] = cmd;
	
	return 0; //TODO ERROR
}

int CommandsContainerShrink(CommandsContainer* container)
{
	assert(container != nullptr);
	
	size_t size = container->file->ncommands;
	
	return CommandsContainerReserve(container, size);
}

void CommandsContainerDeInit(CommandsContainer* container)
{
	assert(container);
	
	free(container);
}

int process_tokens(	const char **tokens, 
                    size_t ntokens, 
					size_t nline, 
                    void* arg)
{
    assert(tokens);
    
	if (ntokens == 0) return 0;
		
	int id = get_command_id(tokens[0]);
	
	if (id < 0) {
		printf("## ERROR: Unknown command \"%s\" on line %lu\n", 
			tokens[0], nline);
		
		return 1;
	}
	
	CommandsContainer* container = 
		reinterpret_cast<CommandsContainer*>(arg);
		
	return get_processor(id)(tokens, ntokens, container);
}

BinaryFile* BinaryFileFromVMFile(const char* fname)
{
    assert(fname);
    
    char* data = read_file_str(fname);
    
    CommandsContainer* container = CommandsContainerInit();
	
	int error = tokenize_lines(	data, 
								" \t;,.", 
								process_tokens, 
								container
								);
    
    CommandsContainerShrink(container);
    
    BinaryFile* retval = container->file;
                                
    CommandsContainerDeInit(container);
    
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

