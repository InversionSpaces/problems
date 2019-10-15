#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "exitingalloc.h"
#include "commands.h"
#include "tokenizer.h"
#include "binaryfile.h"
#include "files.h"

int process_tokens(	const char **tokens, size_t ntokens, 
					size_t nline, void* arg)
{
	if (ntokens == 0)
		return 0;
		
	int id = get_command_id(tokens[0]);
	
	if (id < 0) {
		printf("## ERROR: Unknown command \"%s\" on line %lu\n", 
			tokens[0], nline);
		
		return 1;
	}
	
	CommandsContainer* container = 
		reinterpret_cast<CommandsContainer*>(arg);
		
	return cmd_processors[id](tokens, ntokens, container);
}

int vm_to_binary(const char* inname, const char* outname)
{
	char* file = read_file_str(inname);
	
	CommandsContainer* container = CommandsContainerInit();
	
	int error = tokenize_lines(	file, 
								" \t;,.", 
								process_tokens, 
								container
								);
								
	if (!error) CommandsContainerToFile(container, outname);
	
	CommandsContainerDeInit(container);
	
	return error;
}

int main()
{
	int error = vm_to_binary("test.vm", "out.bin");
	
	if (!error) printf("Done asm\n");
	else {
		printf("Error asm\n");
		
		return 1;
	}
		
	BinaryFile* file = BinaryFileFromFile("out.bin");
	
	printf("Done disasm\n");
	
	for (size_t i = 0; i < file->ncommands; ++i) {
		printf("%s\n", cmd_names[get_command_id(file->commands[i].type)]);
	}
}
