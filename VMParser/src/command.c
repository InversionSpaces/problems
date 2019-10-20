#include <inttypes.h>

#include "foreachmacro.h"
#include "command.h"

#include "binaryfile.h"
#include "cpu.h"

#define NAME_STRING(...) EVAL_STRING(GET_1(__VA_ARGS__)),

#define BINARY(...)	GET_2(__VA_ARGS__)
#define BINARY_COMMA(...) BINARY(__VA_ARGS__),

#define PARGS(...) EVAL_UNTUPLE(GET_4(__VA_ARGS__))
#define EARGS(...) EVAL_UNTUPLE(GET_5(__VA_ARGS__))

#define PROCESSOR_NAME(...) EVAL_CONCAT(process_, GET_1(__VA_ARGS__))
#define PROCESSOR_NAME_COMMA(...) PROCESSOR_NAME(__VA_ARGS__),

#define EXECUTOR_NAME(...) EVAL_CONCAT(execute_, GET_1(__VA_ARGS__))
#define EXECUTOR_NAME_COMMA(...) EXECUTOR_NAME(__VA_ARGS__),

#define PROCESSOR_FUNC(...) 										\
int PROCESSOR_NAME(__VA_ARGS__)	PROCESSOR_FUNC_ARGS					\
{																	\
	if (argc != GET_3(__VA_ARGS__)) return 1;						\
	BinCommand cmd = {	BINARY(__VA_ARGS__),  PARGS(__VA_ARGS__)};	\
	return CommandsContainerAdd(container, cmd);					\
}

#define PUSH(x) PStackPush(stk, x)
#define POP(x) PStackPop(stk, &x);

#define ARG1 cmd.arg1
#define ARG2 cmd.arg2

#define EXECUTOR_FUNC(...)											\
int EXECUTOR_NAME(__VA_ARGS__) EXECUTOR_FUNC_ARGS					\
{																	\
	PStack_t* stk = cpu->stack;										\
	stack_el_t a = 0; 												\
	stack_el_t b = 0;												\
	EARGS(__VA_ARGS__)												\
	return 0;														\
}

#define DECLARE_COMMANDS(...)							\
FOR_EACH(PROCESSOR_FUNC, __VA_ARGS__)					\
FOR_EACH(EXECUTOR_FUNC, __VA_ARGS__)					\
const char* cmd_names[] = {								\
	FOR_EACH(NAME_STRING, __VA_ARGS__)					\
};														\
const uint8_t cmd_binaries[] = {						\
	FOR_EACH(BINARY_COMMA, __VA_ARGS__)					\
};														\
int (*cmd_processors[]) PROCESSOR_FUNC_ARGS = {			\
	FOR_EACH(PROCESSOR_NAME_COMMA, __VA_ARGS__)			\
};														\
int (*cmd_executors[]) EXECUTOR_FUNC_ARGS = {			\
	FOR_EACH(EXECUTOR_NAME_COMMA, __VA_ARGS__)			\
};


DECLARE_COMMANDS(	(PUSH, 	0xFA, 	2, 	(atoi(args[1]), 0), (PUSH(ARG1);)), 
					(POP, 	0xFB, 	1, 	(0, 			0),	(POP(a);)),
					(MUL,	0xFC,	1,	(0, 			0),	(POP(a); POP(b); PUSH(a * b);)),
					(DIV,	0xFD,	1,	(0, 			0),	(POP(a); POP(b); PUSH(a / b);)),
					(ADD,	0xFE,	1,	(0, 			0),	(POP(a); POP(b); PUSH(a + b);)),
					(SUB,	0xFF,	1,	(0, 			0),	(POP(a); POP(b); PUSH(a - b);))
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

int (*get_processor(int id)) PROCESSOR_FUNC_ARGS
{
	return cmd_processors[id];
}

int (*get_executor(int id)) EXECUTOR_FUNC_ARGS
{
	return cmd_executors[id];
}

const char* get_command_name(int id)
{
	return cmd_names[id];
}

uint8_t get_command_binary(int id)
{
    return cmd_binaries[id];
}
