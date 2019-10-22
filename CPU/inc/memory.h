#pragma once

#include "stack.h"

struct Memory;

typedef struct Memory Memory;

Memory* MemoryInit();
				
int get_mem_id(const char* name);

int MemorySet(Memory* mem, int mem_id, int offset, stack_el_t val);

int MemoryGet(Memory* mem, int mem_id, int offset, stack_el_t* val);

void MemoryDeInit(Memory* mem);
