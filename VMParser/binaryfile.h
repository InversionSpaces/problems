#pragma once

#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "exitingalloc.h"

struct Command {
	uint8_t type;
	int32_t arg1;
	int32_t arg2;
};

typedef struct Command Command;

struct BinaryFile {
	size_t ncommands;
	
	Command commands[1];
};

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

void CommandsContainerReserve(CommandsContainer* container, size_t size)
{
	assert(container != nullptr);
	assert(size > 0);
	
	container->file = reinterpret_cast<BinaryFile*>(
		exiting_realloc(container->file, sizeof(BinaryFile) + 
			(size - 1) * sizeof(Command))
	);
	
	container->capacity = size;
}

void CommandsContainerAdd(CommandsContainer* container, Command cmd)
{
	assert(container != nullptr);
	
	if (container->file->ncommands == container->capacity)
		CommandsContainerReserve(container, container->capacity * 2);
		
	container->file->commands[container->file->ncommands++] = cmd;
}

void CommandsContainerShrink(CommandsContainer* container)
{
	assert(container != nullptr);
	
	CommandsContainerReserve(container, container->file->ncommands);
}

void CommandsContainerDeInit(CommandsContainer* container)
{
	assert(container != nullptr);
	
	free(container);
}
