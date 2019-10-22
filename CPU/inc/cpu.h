#pragma once

#include "binaryfile.h"
#include "stack.h"
#include "memory.h"

struct CPU {
	Memory* memory;
	
	PStack_t* rstack;
	
	PStack_t* stack;
	
	size_t fetcher;
	BinaryFile* code;
};

typedef struct CPU CPU;

CPU* CPUInit(BinaryFile* code);

int CPUExecute(CPU* cpu);

void CPUDeInit(CPU* cpu);

