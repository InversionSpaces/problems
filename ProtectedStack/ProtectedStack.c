#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef int stack_el_t;

struct PStack_t {
	size_t capacity;
	size_t size;
	
	stack_el_t* array;
	
#ifndef PS_NDEBUG
	const char* name;
#endif
};

typedef struct PStack_t PStack_t;

typedef enum {
	NO_ERROR 		= 1,
	NULL_STACKP		= 1 << 1,
	NULL_ARRAYP 	= 1 << 2,
	NULL_CAPACITY 	= 1 << 3,
	TOO_BIG_SIZE	= 1 << 4
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

#ifndef PS_NDEBUG
#define PS_ASSERT(stackp, reason) {					\
	PS_ERROR err = PStackCheck(stackp, reason);		\
	if (err & ~NO_ERROR) {							\
		PStackDump(stackp, err);					\
		exit(1);									\
	}												\
}
#else
#define PS_ASSERT(stackp, reason) ;
#endif

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
		
	return retval;
}

void PStackDump(PStack_t* stackp, PS_ERROR error)
{
	printf("## STACK DUMP;\n");
	
	if (error == NO_ERROR)
		printf("## NO ERROR\n");
	if (error & NULL_STACKP)
		printf("## NULL STACK POINTER\n");
	if (error & NULL_ARRAYP)
		printf("## NULL ARRAY POINTER\n");
	if (error & NULL_CAPACITY)
		printf("## NULL CAPACITY\n");
	if (error & TOO_BIG_SIZE)
		printf("## TOO BIG SIZE\n");
		
	
}

void PStackInit(PStack_t* stackp, size_t capacity)
{
	PS_ASSERT(stackp, BEFORE_INIT)
	
	stackp->capacity 	= capacity;
	stackp->size 		= 0;
	stackp->array 		= calloc(capacity, sizeof(stack_el_t));
	
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
	PStackInit(&s, 15);
	
	for (int i = 0; i < 10; ++i)
		PStackPush(&s, i);
		
	//s.size 		= 20;
	//s.array 	= NULL;
		
	for (int i = 0; i < 10; ++i)
		printf("%d ", PStackPop(&s));
}
