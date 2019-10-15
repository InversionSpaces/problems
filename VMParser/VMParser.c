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

int main()
{
	const char* fname = "test.vm";
	
	char* file = read_file(fname);
	
	CommandsContainer* container = CommandsContainerInit();
	
	int error = tokenize_lines(	file, 
								" \t;,.", 
								process_tokens, 
								container
								);
								
	if (!error) {
		CommandsContainerToFile(container, "out.bin");
		printf("Done\n");
	}
	else {
		printf("Error\n");
	}
	
	CommandsContainerDeInit(container);
}
