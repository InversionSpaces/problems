#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "command.h"
#include "tokenizer.h"
#include "binaryfile.h"
#include "files.h"
#include "cpu.h"

int main()
{
	BinaryFile* file = BinaryFileFromVMFile("test.vm");
	
	if (file == 0) {
		printf("Error disasm\n");
		
		return 1;
	}
	
	printf("Done disasm\n");
	
	CPU* cpu = CPUInit(file);
	
	int error = CPUExecute(cpu);
	
	if (error) {
		printf("Error executing\n");
		
		CPUDeInit(cpu);
		
		return 1;
	}
	
	int empty = 1;
	PStackIsEmpty(cpu->stack, &empty);
	int i = 0;
	while (!empty) {
		stack_el_t a = 0;
		PStackPop(cpu->stack, &a);
		printf("%d:\t|%d|\n", i, a);
		PStackIsEmpty(cpu->stack, &empty);
	}
	
	CPUDeInit(cpu);
}
