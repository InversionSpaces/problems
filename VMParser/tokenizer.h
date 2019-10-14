#pragma once

#include "exitingalloc.h"

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
					void* arg )
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
