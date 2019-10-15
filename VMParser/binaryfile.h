#pragma once

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "exitingalloc.h"
#include "files.h"

#pragma pack(push, 1)
struct Command {
	uint8_t type;
	int32_t arg1;
	int32_t arg2;
};
#pragma pack(pop)

typedef struct Command Command;

#pragma pack(push, 1)
struct BinaryFile {
	size_t ncommands;
	
	Command commands[1];
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
	
	container->file = reinterpret_cast<BinaryFile*>(
		exiting_realloc(container->file, sizeof(BinaryFile) + 
			(size - 1) * sizeof(Command))
	);
	
	container->capacity = size;
	
	return 0; //TODO ERROR
}

int CommandsContainerAdd(CommandsContainer* container, Command cmd)
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
	size_t size = sizeof(BinaryFile) + (n - 1) * sizeof(Command);
	
	write_file(container->file, size, filename);
	
	return 0; //TODO ERROR
}

void CommandsContainerDeInit(CommandsContainer* container)
{
	assert(container != nullptr);
	
	free(container);
}
