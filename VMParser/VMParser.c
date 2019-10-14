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


FILE *exiting_fopen(const char *fname, const char *mod)
{
	assert(fname != NULL);
	assert(mod != NULL);
	
	FILE *retval = fopen(fname, mod);
	
	if (retval == NULL) {
		printf("# ERROR: Failed to open file: %s. Exiting...\n", fname);
		exit(1);
	}

	return retval;
}

size_t file_size(FILE *fp)
{
	assert(fp != NULL);

	size_t retval = 0;
	
#ifdef __unix__
	struct stat st;
	int err = fstat(fileno(fp), &st);
	if (err != 0) {
#endif
		fseek(fp, 0L, SEEK_END);
		retval = ftell(fp);
		fseek(fp, 0L, SEEK_SET);
#ifdef __unix__
	}
	retval = st.st_size;
#endif

	return retval;
}

char *read_file(const char *filename)
{
	assert(filename != NULL);

	FILE *fp = exiting_fopen(filename, "rb");
	size_t size = file_size(fp);

	char *retval = reinterpret_cast<char*>(exiting_malloc(size + 1));

	size_t readed = fread(retval, 1, size, fp);
	if (readed != size) {
		printf("# ERROR: Failed to read file: %s. \
					Exiting...\n", filename);
		fclose(fp);
		exit(2);
	}
	
	retval[size] = '\0';

	fclose(fp);

	return retval;
}

int process_tokens(	const char **tokens, size_t ntokens, 
					size_t nline, void* arg)
{
	if (ntokens == 0)
		return 0;
		
	int id = get_command_id(tokens[0]);
	
	if (id < 0) {
		printf("## ERROR: Unknown command \"%s\" on line %lu\n", 
			tokens[0], nline);
		
		return -1;
	}
	
	/*
	printf("|line: %lu; ntokens: %lu; id: %d|\n", nline, ntokens, id);
	
	for (size_t i = 0; i < ntokens; ++i)
		printf("|%s|", tokens[i]);
	printf("\n");
	*/
	
	
	CommandsContainer* container = reinterpret_cast<CommandsContainer*>(
		arg
	);
	return processors[id](tokens, ntokens, container);
}

int main()
{
	const char* fname = "test.vm";
	
	char *file = read_file(fname);
	
	CommandsContainer* container = CommandsContainerInit();
	
	tokenize_lines(	file, " \t;,.", process_tokens, container);
	
	CommandsContainerShrink(container);
	
	for (int i = 0; i < container->file->ncommands; ++i)
		printf("%d %d %d\n", 	container->file->commands[i].type,
								container->file->commands[i].arg1, 
								container->file->commands[i].arg2);
	
	printf("Done\n");
}
