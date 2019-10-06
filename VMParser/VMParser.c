#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

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

void *exiting_calloc(size_t n, size_t size)
{
	void *retval = calloc(n, size);
	if (retval == NULL) {
		printf("# ERROR: Failed to calloc memory. Exiting...\n");
		exit(2);
	}

	return retval;
}

void *exiting_malloc(size_t size)
{
	void *retval = malloc(size);
	if (retval == NULL) {
		printf("# ERROR: Failed to malloc memory. Exiting...\n");
		exit(2);
	}

	return retval;
}

void *exiting_realloc(void *ptr, size_t size)
{
	void *retval = realloc(ptr, size);
	if (retval == NULL) {
		printf("# ERROR: Failed to realloc memory. Exiting...\n");
		exit(2);
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

char *next_line(char *cur_line)
{
	assert(cur_line != NULL);
	
	for (; *cur_line; ++cur_line) {
		if (*cur_line == '\n') {
			*cur_line = '\0';
			return (++cur_line);
		}
	}
	
	return NULL;
}

#define TOKENS_CHUNK (5)

int tokenize_lines(	char *data, const char *delim,
					int (*ptk)(const char**, size_t, size_t, void*), 
					void* arg)
{
	assert(data != NULL);
	assert(delim != NULL);
	assert(ptk != NULL);
	
	int retval = 0;
	
	size_t tokens_size = TOKENS_CHUNK;
	const char** tokens = reinterpret_cast<const char**>
				(exiting_calloc(tokens_size, sizeof(char*)));
				
	char *cur_line = data;
	for (size_t nline = 0; cur_line != NULL; ++nline) {
		char *new_line = next_line(cur_line);
		
		size_t ntokens = 0;
		for (	char *token = strtok(cur_line, delim);
				token != NULL;
				token = strtok(NULL, delim)) {
			
			if (ntokens == tokens_size) {
				tokens_size += TOKENS_CHUNK;
				tokens = reinterpret_cast<const char**>(
					exiting_realloc(tokens, tokens_size * sizeof(char*))
				);
			}
			
			tokens[ntokens++] = token;
		}
		
		retval = ptk(tokens, ntokens, nline, arg);
		if (retval != 0)
			break;
		
		cur_line = new_line;
	}
	
	free(tokens);
	
	return retval;
}

const char *commands[] = {
	"push",
	"pop",
	"add",
	"mul",
	"div"
};

int get_command_id(const char *name)
{
	for (int i = 0; i < sizeof(commands) / sizeof(*commands); ++i) {
		if (strcmp(commands[i], name) == 0)
			return i;
	}
	
	return -1;
}

int process_push(const char* args[], size_t argc)
{
	printf("push\n");
	return 0;
}

int process_add(const char* args[], size_t argc)
{
	printf("add\n");
	return 0;
}

int process_mul(const char* args[], size_t argc)
{
	printf("mul\n");
	return 0;
}

int process_div(const char* args[], size_t argc)
{
	printf("div\n");
	return 0;
}

int process_pop(const char* args[], size_t argc)
{
	printf("pop\n");
	return 0;
}

int (*processors[]) (const char**, size_t) = {
	process_push,
	process_pop,
	process_add,
	process_mul,
	process_div
};

int process_tokens(	const char **tokens, size_t ntokens, 
					size_t nline, void *arg)
{
	if (ntokens == 0)
		return 0;
		
	int id = get_command_id(tokens[0]);
	
	if (id < 0) {
		printf("## ERROR: Unknown command %s on line %lu\n", 
			tokens[0], nline);
		
		return -1;
	}
	
	/*
	printf("|line: %lu; ntokens: %lu; id: %d|\n", nline, ntokens, id);
	
	for (size_t i = 0; i < ntokens; ++i)
		printf("|%s|", tokens[i]);
	printf("\n");
	*/
	
	int res = processors[id](tokens, ntokens);
	
	return 0;
}

int main()
{
	const char* fname = "test.vm";
	
	char *file = read_file(fname);
	
	tokenize_lines(	file, " \t;,.", process_tokens, NULL);
	
	printf("Done\n");
}
