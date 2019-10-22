#include <assert.h>

#include "cpu.h"

#include "command.h"
#include "exitingalloc.h"

#define INITIAL_SIZE (128)

CPU* CPUInit(BinaryFile* code)
{
	assert(code);
	
	CPU* retval = reinterpret_cast<CPU*>(
						exiting_malloc(sizeof(CPU))
					);
	
	retval->fetcher = 0;
	retval->code = code;
	retval->stack = reinterpret_cast<PStack_t*>(
						exiting_malloc(sizeof(PStack_t))
					);
	
	PS_ERROR error = PStackInitMACRO(retval->stack, INITIAL_SIZE);
	if (error != NO_ERROR) {
		printf("## Error: failed to init stack\n");
		
		free(retval);
		retval = 0;
	}
	
	retval->memory = MemoryInit();
	
	return retval;
}

int CPUExecute(CPU* cpu)
{
	while (cpu->fetcher < cpu->code->ncommands) {
		BinCommand cmd = cpu->code->commands[cpu->fetcher];
		int id = get_command_id(cmd.type);
		
		if (id < 0) {
			printf("## Error: unknown command\n");
			
			return 1;
		}
		
		//printf("%s:\t%d\t%d\n", get_command_name(id), cmd.arg1, cmd.arg2);
		
		int error = get_executor(id)(cpu, cmd);
		
		if (error) return error;
	}
	
	return 0;
}

void CPUDeInit(CPU* cpu)
{
	assert(cpu);
	
	free(cpu->code);
	MemoryDeInit(cpu->memory);
	PStackDeInit(cpu->stack);
	free(cpu);
}
