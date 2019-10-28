#include <stdio.h>
#include <string.h>

#include "binaryfile.h"

inline int print_usage(const char* name)
{
	printf("## Aassembler for .vm files\n");
	printf("## By InversionSpaces\n");
	printf("## Translates VM_FILE and writes BIN_FILE\n");
	printf("## Usage: %s VM_FILE BIN_FILE\n", name);
	
	return 0;
}

int main(int argc, char* argv[])
{
	if (argc != 3)
		return print_usage(argv[0]);
	
	BinaryFile* file = BinaryFileFromVMFile(argv[1]);
	
	int error = BinaryFileToFile(file, argv[2]);
	
	if (error) {
		printf("## Error writing file\n");
		
		return 1;
	}

	printf("## Done\n");
	
	return 0;
}
