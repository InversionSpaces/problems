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

int BinaryFileToFile(BinaryFile* file, const char* fname);


typedef struct LabelEntry LabelEntry;

struct LabelEntry
{
	const char* name;
	int ncommand;
}; 

struct CommandsContainer {
	size_t lsize;
	size_t lcapacity;
	LabelEntry* labels;
	
	size_t ccapacity;
	BinaryFile* file;
};

typedef struct CommandsContainer CommandsContainer;

int CContainerPushLabels(CommandsContainer* container);
int CContainerAdd(CommandsContainer* container, BinCommand cmd);

int CContainerLabelSet(CommandsContainer* container, const char* name, int ncommand);
int CContainerLabelGet(CommandsContainer* container, const char* name);
