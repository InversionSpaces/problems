#pragma once

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "exitingalloc.h"
#include "files.h"
#include "commands.h"

#pragma pack(push, 1)
struct BinaryFile {
	size_t ncommands;
	
	BinCommand commands[1];
};
#pragma pack(pop)

typedef struct BinaryFile BinaryFile;

struct CommandsContainer {
	size_t capacity;
	
	BinaryFile* file;
};

typedef struct CommandsContainer CommandsContainer;

CommandsContainer* CommandsContainerInit() 
{
	CommandsContainer* retval = reinterpret_cast<CommandsContainer*>(
		exiting_malloc(sizeof(CommandsContainer))
	);
	
	retval->capacity = 1;
	retval->file = reinterpret_cast<BinaryFile*>(
		exiting_malloc(sizeof(BinaryFile))
	);
	
	retval->file->ncommands = 0;
	
	return retval;
}

int CommandsContainerReserve(CommandsContainer* container, size_t size)
{
	assert(container != nullptr);
	assert(size > 0);
	
	if (container->capacity == size)
		return 0;
	
	size_t bsize = sizeof(BinaryFile) + (size - 1) * sizeof(BinCommand);
	container->file = reinterpret_cast<BinaryFile*>(
		exiting_realloc(container->file, bsize)
	);
	
	container->capacity = size;
	
	return 0; //TODO ERROR
}

int CommandsContainerAdd(CommandsContainer* container, BinCommand cmd)
{
	assert(container != nullptr);
	
	if (container->file->ncommands == container->capacity)
		CommandsContainerReserve(container, container->capacity * 2);
		
	container->file->commands[container->file->ncommands++] = cmd;
	
	return 0; //TODO ERROR
}

int CommandsContainerShrink(CommandsContainer* container)
{
	assert(container != nullptr);
	
	size_t size = container->file->ncommands;
	
	return CommandsContainerReserve(container, size);
}

int CommandsContainerToFile(CommandsContainer* container, 
								const char* filename)
{
	//CommandsContainerShrink(container);
	
	size_t n = container->file->ncommands;
	size_t size = sizeof(BinaryFile) + (n - 1) * sizeof(BinCommand);
	
	write_file(container->file, size, filename);
	
	return 0; //TODO ERROR
}

BinaryFile* BinaryFileFromFile(const char* fname)
{
	assert(fname);
	
	void* ptr = 0;
	size_t size = read_file_bin(fname, &ptr);
	
	BinaryFile* retval = reinterpret_cast<BinaryFile*>(ptr);
	
	size_t expected = sizeof(BinaryFile) + 
			(retval->ncommands - 1) * sizeof(BinCommand);
				
	if (expected != size) {
		printf("## Error reading binary file: size doesn't match\n");
		
		retval = 0;
	}
	
	return retval;
}

void CommandsContainerDeInit(CommandsContainer* container)
{
	assert(container != nullptr);
	
	free(container);
}
