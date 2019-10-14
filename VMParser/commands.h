#pragma once

#include "foreachmacro.h"
#include "binaryfile.h"

#define STRING(x) #x,

#define PROCESSOR_FUNC(x) process_##x,

#define EXECUTE_FUNC(x) execute_##x,

#define ENUMERATOR(x) CMD_ID_##x,

#define DECLARE_COMMANDS(...) 											\
	enum CommandId {													\
		FOR_EACH(ENUMERATOR, __VA_ARGS__)								\
	};																	\
	const char* const names[] = {										\
		FOR_EACH(STRING, __VA_ARGS__)									\
	};																	\
	int (*const processors[])(const char**, size_t, 					\
					CommandsContainer* container) = {					\
		FOR_EACH(PROCESSOR_FUNC, __VA_ARGS__)							\
	};																	\
	int (*const executers[])() = {										\
		FOR_EACH(EXECUTE_FUNC, __VA_ARGS__)								\
	};																	

#include "processors.h"
#include "executors.h"

DECLARE_COMMANDS(push, pop, add, mul, div)

int get_command_id(const char *name)
{
	for (int i = 0; i < sizeof(names) / sizeof(0[names]); ++i) {
		if (strcmp(names[i], name) == 0)
			return i;
	}
	
	return -1;
}
