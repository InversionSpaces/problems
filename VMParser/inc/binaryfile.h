#pragma once

#include <stdlib.h>
#include <inttypes.h>

#include "bcommand.h"

#pragma pack(push, 1)
struct BinaryFile {
	size_t ncommands;
	
	BinCommand commands[1];
};
#pragma pack(pop)

typedef struct BinaryFile BinaryFile;

BinaryFile* BinaryFileFromBinFile(const char* fname);
BinaryFile* BinaryFileFromVMFile(const char* fname);


struct CommandsContainer {
	size_t capacity;
	
	BinaryFile* file;
};

typedef struct CommandsContainer CommandsContainer;

int CommandsContainerAdd(CommandsContainer* container, BinCommand cmd);
