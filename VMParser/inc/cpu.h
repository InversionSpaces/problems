#pragma once

#include "binaryfile.h"
#include "stack.h"

struct CPU {
	size_t fetcher;
	
	PStack_t* stack;
	
	BinaryFile* code;
};

typedef struct CPU CPU;

CPU* CPUInit(BinaryFile* code);

int CPUExecute(CPU* cpu);

