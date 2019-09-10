#include <stdio.h>
#include <stdlib.h>
#include <memory.h>

typedef struct stack stack;

struct stack {
	size_t elsize;
	void* bottom;
	void* top;
	size_t size;
};

#define INITIAL_STACK_SIZE 	(32)
#define STACK_CHUNK_SIZE 	(32)

stack* stack_init(size_t elsize) {
	stack* retval = (stack*)malloc(sizeof(stack));
	retval->elsize = elsize;
	retval->size = INITIAL_STACK_SIZE * elsize;
	retval->bottom = malloc(retval->size);
	retval->top = retval->bottom;
}

void stack_push(stack* s, void* p) {
	if (s->size == (size_t)(s->top - s->bottom)) 
	{
		size_t nsize = s->size + STACK_CHUNK_SIZE * s->elsize;
		s->bottom = realloc(s->bottom, nsize);
		s->top = s->bottom + s->size;
		s->size = nsize;
	}
	memcpy(s->top, p, s->elsize);
	s->top += s->elsize;
}

void stack_pop(stack* s, void* p) {
	s->top -= s->elsize;
	memcpy(p, s->top, s->elsize);
}

void stack_deinit(stack* s) {
	free(s->bottom);
	free(s);
}

char stack_is_empty(stack* s) {
	return (s->top == s->bottom);
}

int main() {
}
