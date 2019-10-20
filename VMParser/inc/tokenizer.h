#pragma once

#include <stdlib.h>

int tokenize_lines(	char *data, const char *delim,
					int (*ptk)(const char**, size_t, size_t, void*),
					void* arg );
