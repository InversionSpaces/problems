#pragma once

#include <inttypes.h>

#include "foreachmacro.h"

#pragma pack(push, 1)
struct BinCommand {
	uint8_t type;
	int32_t arg1;
	int32_t arg2;
};
#pragma pack(pop)

typedef struct BinCommand BinCommand;

#include "binaryfile.h"

#define CMD_ID(...) EVAL_CONCAT(CMD_ID_, GET_1(__VA_ARGS__)),

#define NAME_STRING(...) EVAL_STRING(GET_1(__VA_ARGS__)),

#define BINARY(...)	GET_2(__VA_ARGS__)
#define BINARY_COMMA(...) BINARY(__VA_ARGS__),

#define ARGS(...) EVAL_UNTUPLE(GET_4(__VA_ARGS__))

#define PROCESSOR_NAME(...) EVAL_CONCAT(process_, GET_1(__VA_ARGS__))
#define PROCESSOR_NAME_COMMA(...) PROCESSOR_NAME(__VA_ARGS__),

#define EXECUTOR_NAME(...) EVAL_CONCAT(execute_, GET_1(__VA_ARGS__))
#define EXECUTOR_NAME_COMMA(...) EXECUTOR_NAME(__VA_ARGS__),

#define PROCESSOR_FUNC_ARGS \
(const char* args[], size_t argc, CommandsContainer* container)

#define EXECUTOR_FUNC_ARGS \
()

#define PROCESSOR_FUNC(...) 										\
int PROCESSOR_NAME(__VA_ARGS__)	PROCESSOR_FUNC_ARGS					\
{																	\
	if (argc != GET_3(__VA_ARGS__)) return 1;						\
	BinCommand cmd = {	BINARY(__VA_ARGS__),  ARGS(__VA_ARGS__)};	\
	return CommandsContainerAdd(container, cmd);					\
}

#define EXECUTOR_FUNC(...)											\
int EXECUTOR_NAME(__VA_ARGS__)										\
()																	\
{																	\
}

#define DECLARE_COMMANDS(...)							\
enum CommandID {										\
	FOR_EACH(CMD_ID, __VA_ARGS__)						\
};														\
FOR_EACH(PROCESSOR_FUNC, __VA_ARGS__)					\
FOR_EACH(EXECUTOR_FUNC, __VA_ARGS__)					\
const char* cmd_names[] = {								\
	FOR_EACH(NAME_STRING, __VA_ARGS__)					\
};														\
const uint8_t cmd_binaries[] = {						\
	FOR_EACH(BINARY_COMMA, __VA_ARGS__)					\
};														\
int (*cmd_processors[]) PROCESSOR_FUNC_ARGS  = {		\
	FOR_EACH(PROCESSOR_NAME_COMMA, __VA_ARGS__)			\
};


DECLARE_COMMANDS(	(PUSH, 	0xFA, 	2, 	(atoi(args[1]), 0)), 
					(POP, 	0xFB, 	1, 	(0, 0)),
					(MUL,	0xFC,	1,	(0, 0)),
					(DIV,	0xFD,	1,	(0, 0)),
					(ADD,	0xFE,	1,	(0, 0)),
					(SUB,	0xFF,	1,	(0, 0))
				)

#define SIZE(x) (sizeof(x) / sizeof(0[x]))

int get_command_id(const char *name)
{
	assert(name);
	
	for (int i = 0; i < SIZE(cmd_names); ++i) {
		if (strcmp(cmd_names[i], name) == 0)
			return i;
	}
	
	return -1;
}

int get_command_id(const uint8_t hex)
{
	for (int i = 0; i < SIZE(cmd_binaries); ++i) {
		if (cmd_binaries[i] == hex)
			return i;
	}
	
	return -1;
}
