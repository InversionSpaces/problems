#include <inttypes.h>
#include <stdio.h>

#include "foreachmacro.h"
#include "command.h"

#include "binaryfile.h"
#include "cpu.h"

#define SIZE(x) (sizeof(x) / sizeof(0[x]))

//======================================================================

#define NAME_STRING(...) EVAL_STRING(GET_1(__VA_ARGS__)),

#define BINARY(...)	GET_2(__VA_ARGS__)
#define BINARY_COMMA(...) BINARY(__VA_ARGS__),

#define PCODE(...) EVAL_UNTUPLE(GET_4(__VA_ARGS__))
#define ECODE(...) EVAL_UNTUPLE(GET_5(__VA_ARGS__))

#define PROCESSOR_NAME(...) EVAL_CONCAT(process_, GET_1(__VA_ARGS__))
#define PROCESSOR_NAME_COMMA(...) PROCESSOR_NAME(__VA_ARGS__),

#define EXECUTOR_NAME(...) EVAL_CONCAT(execute_, GET_1(__VA_ARGS__))
#define EXECUTOR_NAME_COMMA(...) EXECUTOR_NAME(__VA_ARGS__),

#define PROCESSOR_FUNC(...) 										\
int PROCESSOR_NAME(__VA_ARGS__)	PROCESSOR_FUNC_ARGS					\
{																	\
	if (argc != GET_3(__VA_ARGS__)) return 1;						\
	uint8_t hex = BINARY(__VA_ARGS__);								\
	PCODE(__VA_ARGS__)												\
}

#define EXECUTOR_FUNC(...)											\
int EXECUTOR_NAME(__VA_ARGS__) EXECUTOR_FUNC_ARGS					\
{																	\
	ECODE(__VA_ARGS__)												\
}

//======================================================================

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

//======================================================================

#define JMP_FUNC_NAME(...) EVAL_CONCAT(jmp_, GET_1(__VA_ARGS__))

#define JMP_FUNC_NAME_COMMA(...) JMP_FUNC_NAME(__VA_ARGS__),

#define JMP_FUNC_ARGS (CPU* cpu, BinCommand cmd)

//======================================================================

#define JMP_FUNC(...) 							\
int JMP_FUNC_NAME(__VA_ARGS__) JMP_FUNC_ARGS	\
	GET_3(__VA_ARGS__)

#define DECLARE_JUMPS(...)								\
FOR_EACH(JMP_FUNC, __VA_ARGS__)							\
const char* jmp_names[] = {								\
	FOR_EACH(NAME_STRING, __VA_ARGS__)					\
};														\
const uint8_t jmp_binaries[] = {						\
	FOR_EACH(BINARY_COMMA, __VA_ARGS__)					\
};														\
int (*jmp_func[])JMP_FUNC_ARGS = {						\
	FOR_EACH(JMP_FUNC_NAME_COMMA, __VA_ARGS__)			\
};

//======================================================================

#define JUMP_IF_OP(OPERATION) 					\
stack_el_t a = 0;								\
int error = PStackPop(cpu->stack, &a);			\
if (error) return error;						\
if (a OPERATION 0) cpu->fetcher = cmd.arg2;		\
else cpu->fetcher++;							\
return 0;

#define JUMP_UN				\
cpu->fetcher = cmd.arg2;	\
return 0;

//======================================================================

DECLARE_JUMPS(

(UN, 0xAA, {
	JUMP_UN
}),
 
(EQ, 0xBB, {
	JUMP_IF_OP(==)
}), 

(GT, 0xCC, {
	JUMP_IF_OP(>)
}), 

(LS, 0xDD, {
	JUMP_IF_OP(<)
}),

(LEQ, 0xEE, {
	JUMP_IF_OP(<=)
}),

(GEQ, 0xEE, {
	JUMP_IF_OP(>=)
})
)

//======================================================================

int get_jmp_id(const char *name)
{
	assert(name);
	
	for (int i = 0; i < SIZE(jmp_names); ++i) {
		if (strcmp(jmp_names[i], name) == 0)
			return i;
	}
	
	return -1;
}

int get_jmp_id(const uint8_t hex)
{
	for (int i = 0; i < SIZE(jmp_binaries); ++i) {
		if (jmp_binaries[i] == hex)
			return i;
	}
	
	return -1;
}

//======================================================================

#define POP_PUSH_OP(OPERATION)					\
stack_el_t a = 0;								\
stack_el_t b = 0;								\
int error = PStackPop(cpu->stack, &a);			\
if (error) return error;						\
error = PStackPop(cpu->stack, &b);				\
if (error) return error;						\
return PStackPush(cpu->stack, a OPERATION b);

#define PUT_CMD 						\
BinCommand cmd = {hex, 0, 0};			\
return CContainerAdd(container, cmd);

//======================================================================

DECLARE_COMMANDS(	

(PUSH, 	0xFA, 	3, 	
({
	int mem_id = get_mem_id(args[1]);
	if (mem_id < 0) return 1;
	// TODO something to not convert mem_id
	BinCommand cmd = {hex, uint8_t(mem_id), atoi(args[2])}; 
	return CContainerAdd(container, cmd);
}), 
({
	cpu->fetcher++;
	if (cmd.arg1 == 0) {
		return PStackPush(cpu->stack, cmd.arg2);
	}
	else {
		stack_el_t val = 0;
		int error = MemoryGet(cpu->memory, cmd.arg1, cmd.arg2, &val);
		if (error) return error;
		return PStackPush(cpu->stack, val);
	}
})), 

(POP, 	0xFB, 	3, 	
({
	int mem_id = get_mem_id(args[1]);
	if (mem_id <= 0) return 1;
	// TODO something to not convert mem_id
	BinCommand cmd = {hex, uint8_t(mem_id), atoi(args[2])};
	return CContainerAdd(container, cmd);
}),	
({
	cpu->fetcher++;
	stack_el_t val = 0;
	int error = PStackPop(cpu->stack, &val);
	if (error) return error;
	return MemorySet(cpu->memory, cmd.arg1, cmd.arg2, val);
})),

(MUL,	0xFC,	1,	
({
	PUT_CMD
}),	
({
	cpu->fetcher++;
	POP_PUSH_OP(*)
})),

(DIV,	0xFD,	1,	
({
	PUT_CMD
}),	
({
	cpu->fetcher++;
	POP_PUSH_OP(/)
})),

(ADD,	0xFE,	1,	
({
	PUT_CMD
}),	
({
	cpu->fetcher++;
	POP_PUSH_OP(+)
})),

(SUB,	0xFF,	1,	
({
	PUT_CMD
}),	
({
	cpu->fetcher++;
	POP_PUSH_OP(-)
})),

(LABEL, 0x00, 	2, 	
({
	return CContainerLabelSet(	container, 
								args[1], 
								container->file->ncommands
								);
}), 
({
	// No cpu command
	return 1;
})),

(JUMP, 0xC1, 	3, 	
({ 
	int id = get_jmp_id(args[1]);
	if (id < 0) return 1; 
	BinCommand cmd = {
				hex,
				jmp_binaries[id],
				CContainerLabelGet(container, args[2])
				};
	return CContainerAdd(container, cmd);
}), 
({
	int id = get_jmp_id(cmd.arg1);
	if (id < 0) return 1;
	return jmp_func[id](cpu, cmd);
}))
)

//======================================================================

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

//======================================================================
