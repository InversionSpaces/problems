#include <string.h>
#include <assert.h>

#include "memory.h"

#include "exitingalloc.h"
#include "foreachmacro.h"

#define ARRAY_DEF(name, size) stack_el_t name[size];

#define NAME(name, ...) name,

#define NAME_STRING(...) EVAL_STRING(GET_1(__VA_ARGS__)),

#define LOC_ID(...) EVAL_CONCAT(LOC_, GET_1(__VA_ARGS__))
#define LOC_ID_COMMA(...) LOC_ID(__VA_ARGS__),

#define LOC_GETTER(...) 				\
if (LOC_ID(__VA_ARGS__) == mem_id) 		\
	return mem->GET_1(__VA_ARGS__);

#define DECLARE_MEMORY(...)									\
struct Memory {												\
	FOR_EACH(ARRAY_DEF, __VA_ARGS__)						\
};															\
const char* mem_names[] = {									\
	FOR_EACH(NAME_STRING, __VA_ARGS__)						\
	"CONSTANT",												\
	"IN",													\
	"OUT",													\
};															\
enum MEM_ID 												\
{															\
	FOR_EACH(LOC_ID_COMMA, __VA_ARGS__)						\
	NOT_MEM_ID												\
};															\
stack_el_t* get_mem_loc(Memory* mem, int mem_id)			\
{															\
	FOR_EACH(LOC_GETTER, __VA_ARGS__)						\
	return 0;												\
}

DECLARE_MEMORY(	
		(LOCAL, 128), 
		(GLOBAL, 128),
		(MEMORY, 1024)
	)

#define SIZE(x) (sizeof(x) / sizeof(0[x]))

Memory* MemoryInit()
{
	Memory* retval = reinterpret_cast<Memory*>(
						exiting_malloc(sizeof(Memory))
					);
					
	return retval;
}

int get_not_mem_id()
{
	return NOT_MEM_ID;
}

int get_mem_id(const char* name)
{
	for (int i = 0; i < SIZE(mem_names); ++i)
		if (strcmp(mem_names[i], name) == 0)
			return i;
	
	return -1;
}

int MemorySet(Memory* mem, int mem_id, int offset, stack_el_t val)
{
	assert(mem);
	
	if ((mem_id >= 0) && (mem_id < NOT_MEM_ID)) {		
		get_mem_loc(mem, mem_id)[offset] = val;
		
		return 0;
	}
	
	return 1;
}

int MemoryGet(Memory* mem, int mem_id, int offset, stack_el_t* val)
{
	assert(mem);
	
	if ((mem_id >= 0) && (mem_id < NOT_MEM_ID)) { 
		*val = get_mem_loc(mem, mem_id)[offset];
		
		return 0;
	}
	
	return 1;
}

void MemoryDeInit(Memory* mem)
{
	free(mem);
}
