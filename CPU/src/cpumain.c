#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>

#include "binaryfile.h"
#include "cpu.h"

inline int print_usage(const char* name)
{
	printf("## CPU for .bin files\n");
	printf("## By InversionSpaces\n");
	printf("## Executes code in BIN_FILE\n");
	printf("## Usage: %s BIN_FILE\n", name);
	
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 2)
		return print_usage(argv[0]);
	
	BinaryFile* file = BinaryFileFromBinFile(argv[1]);
	
	if (file == 0) {
		printf("## Error disasm\n");
		
		return 1;
	}
	
	CPU* cpu = CPUInit(file);
	
	int error = CPUExecute(cpu);
	
	if (error) {
		printf("## Error executing\n");
		
		CPUDeInit(cpu);
		
		return 1;
	}
	
	int empty = 1;
	PStackIsEmpty(cpu->stack, &empty);
	int i = 0;
	while (!empty) {
		stack_el_t a = 0;
		PStackPop(cpu->stack, &a);
		
		printf("## %d:\t|%d|\n", i++, a);
		
		PStackIsEmpty(cpu->stack, &empty);
	}
	
	CPUDeInit(cpu);
}
