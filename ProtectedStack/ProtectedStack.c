#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <inttypes.h>

#ifndef PS_NDEBUG
#define DEAD_BYTE (0x4D)
#define GUARD_BYTE (0xBA)
#endif

typedef int stack_el_t;

struct PStack_t {
#ifndef PS_NDEBUG
	uint8_t front_quard;
	uint8_t hash;
#endif

	size_t capacity;
	size_t size;
	
	stack_el_t* array;
	
#ifndef PS_NDEBUG
	const char* name;
	uint8_t back_quard;
#endif
};

typedef struct PStack_t PStack_t;

typedef enum {
	NO_ERROR 		= 1,
	NULL_STACKP		= 1 << 1,
	NULL_ARRAYP 	= 1 << 2,
	NULL_CAPACITY 	= 1 << 3,
	TOO_BIG_SIZE	= 1 << 4,
	TOO_SMALL_SIZE	= 1 << 5
} PS_ERROR;

typedef enum {
	COMMON			= 1,
	BEFORE_INIT		= 1 << 1,
	AFTER_INIT		= 1 << 2,
	BEFORE_DEINIT	= 1 << 3,
	BEFORE_PUSH		= 1 << 4,
	AFTER_PUSH		= 1 << 5,
	BEFORE_POP		= 1 << 6,
	AFTER_POP		= 1 << 7
} PS_CHECK_REASON;

#define ASSERT_EXIT_CODE (2)

#ifndef PS_NDEBUG
#define PS_ASSERT(stackp, reason) {					\
	PS_ERROR err = PStackCheck(stackp, reason);		\
	if (err & ~NO_ERROR) {							\
		PStackDump(stackp, err);					\
		exit(ASSERT_EXIT_CODE);						\
	}												\
}
#else
#define PS_ASSERT(stackp, reason) ;
#endif

#ifndef PS_NDEBUG
#define PStackInitMACRO(stackp, capacity) {			\
	PStackInit(stackp, capacity);					\
	(stackp)->name = #stackp;						\
}
#else
#define PStackInitMACRO(stackp, capacity) {			\
	PStackInit(stackp, capcity);					\
}
#endif

#define PStackDumpMACRO(stackp) 					\
		PStackDump(stackp, PStackCheck(stackp, COMMON));

PS_ERROR PStackCheck(PStack_t* stackp, PS_CHECK_REASON reason)
{
	PS_ERROR retval = NO_ERROR;
	
	if (reason)
		if (stackp == NULL)
			retval |= NULL_STACKP;
	
	if (reason & ~BEFORE_INIT)
		if (stackp->array == NULL)
			retval |= NULL_ARRAYP;

	if (reason & ~BEFORE_INIT)
		if (stackp->capacity == 0)
			retval |= NULL_CAPACITY;

	if (reason & ~BEFORE_INIT) 
		if (stackp->capacity < stackp->size)
			retval |= TOO_BIG_SIZE;
	
	if (reason & BEFORE_PUSH)
		if (stackp->capacity <= stackp->size)
			retval |= TOO_BIG_SIZE;
			
	if (reason & BEFORE_POP)
		if (stackp->size == 0)
			retval |= TOO_SMALL_SIZE;
		
	return retval;
}

int IsDead(const void* ptr, size_t size) {
	for (uint8_t* tmp = (uint8_t*)ptr; --size; ++tmp)
		if (*tmp != DEAD_BYTE)
			return 0;
	return 1;
}

void PStackDump(PStack_t* stackp, PS_ERROR error)
{
	printf("## STACK DUMP;\n");
	
	if (error & NULL_STACKP) {
		printf("## NULL STACK POINTER;\n");
		return;
	}
	if (error == NO_ERROR) {
		#ifndef PS_NDEBUG
		printf("## Stack name: %s;\n", stackp->name);
		#endif
		
		printf("## NO ERROR;\n");
		printf("## Capacity: %lu; Size: %lu;\n",
				stackp->capacity, stackp->size);
				
		for (size_t i = 0; i < stackp->size; i++) {
			printf("## + [%lu] %d", i, stackp->array[i]);
			if (IsDead(stackp->array + i, sizeof(stack_el_t)))
				printf(" (POSSIBLY DEAD)");
			printf("\n");
		}
		
		for (size_t i = stackp->size; i < stackp->capacity; i++) {
			printf("## - [%lu] %d", i, stackp->array[i]);
			if (IsDead(stackp->array + i, sizeof(stack_el_t)))
				printf(" (DEAD)");
			printf("\n");
		}
	}
	if (error & NULL_ARRAYP) {
		#ifndef PS_NDEBUG
		printf("## Stack name: %s;\n", stackp->name);
		#endif
		
		printf("## NULL ARRAY POINTER\n");
		printf("## Capacity: %lu; Size: %lu;\n",
				stackp->capacity, stackp->size);
	}
	if (error & NULL_CAPACITY) {
		#ifndef PS_NDEBUG
		printf("## Stack name: %s;\n", stackp->name);
		#endif
		
		printf("## NULL CAPACITY\n");
		printf("## Capacity: %lu; Size: %lu;\n",
				stackp->capacity, stackp->size);
	}
	if (error & TOO_BIG_SIZE) {
		#ifndef PS_NDEBUG
		printf("## Stack name: %s;\n", stackp->name);
		#endif
		
		printf("## TOO BIG SIZE\n");
		printf("## Capacity: %lu; Size: %lu;\n",
				stackp->capacity, stackp->size);
	}
	if (error & TOO_SMALL_SIZE) {
		#ifndef PS_NDEBUG
		printf("## Stack name: %s;\n", stackp->name);
		#endif
		
		printf("## TOO SMALL SIZE\n");
		printf("## Capacity: %lu; Size: %lu;\n",
				stackp->capacity, stackp->size);
	}
}

void PStackInit(PStack_t* stackp, size_t capacity)
{
	PS_ASSERT(stackp, BEFORE_INIT)
	
	stackp->capacity 	= capacity;
	stackp->size 		= 0;
	stackp->array 		= calloc(capacity, sizeof(stack_el_t));
	
	memset(stackp->array, DEAD_BYTE, capacity * sizeof(stack_el_t));
	
	PS_ASSERT(stackp, AFTER_INIT)
}

void PStackPush(PStack_t* stackp, stack_el_t elem)
{	
	PS_ASSERT(stackp, BEFORE_PUSH);
	
	stackp->array[stackp->size++] = elem;

	PS_ASSERT(stackp, AFTER_PUSH);
}

stack_el_t PStackPop(PStack_t* stackp)
{
	PS_ASSERT(stackp, BEFORE_POP);
	
	stack_el_t retval = stackp->array[--stackp->size];
	memset(stackp->array + stackp->size, DEAD_BYTE, sizeof(stack_el_t));
	
	PS_ASSERT(stackp, AFTER_POP);
	
	return retval;
}

int PStackIsEmpty(PStack_t* stackp)
{
	PS_ASSERT(stackp, COMMON);
	
	return (stackp->size == 0);
}

void PStackDeInit(PStack_t* stackp)
{
	PS_ASSERT(stackp, BEFORE_DEINIT);
	
	free(stackp->array);
	
	stackp->size 		= 0;
	stackp->capacity 	= 0;
	stackp->array		= NULL;
}

int main()
{
	PStack_t s = {};
	PStackInitMACRO(&s, 15);
	
	for (int i = 0; i < 4; ++i)
		PStackPush(&s, i);
		
	PStackPop(&s);	
	PStackPush(&s, 0x4D4D4D4D);
	
	PStackDumpMACRO(&s);
		
	for (int i = 0; i < 4; ++i)
		printf("%d ", PStackPop(&s));
	printf("\n");
		
	PStackDumpMACRO(&s);
}
