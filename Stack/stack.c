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

void swap(int* a, int* b)
{
	int tmp = *a;
	*a = *b;
	*b = tmp;
}

void q_sort(int* arr, size_t size) {
	stack* st = stack_init(sizeof(int*));
	
	int* l = arr;
	int* r = arr + size - 1;
	
	int* ls = NULL;
	int* rs = NULL;
	int* p = NULL;
	
	stack_push(st, &r);
	stack_push(st, &l);
	
	while (!stack_is_empty(st)) 
	{		
		stack_pop(st, &l);
		stack_pop(st, &r);
		
		size_t tsize = r - l + 1;
		
		if (tsize < 2) 
			continue;
		
		ls = l;
		rs = r;
		p = l + tsize / 2;
		
		while (l < r) {
			while (*p > *l && l < p)
				++l;
			while (*p <= *r && r > p)
				--r;
				
			if (l == r) break;
		
			swap(l, r);
			
			if (p == l)
				p = r;
			else if (p == r)
				p = l;
		}
		
		if (p > ls)
		{
			--p;
			stack_push(st, &p);
			++p;
			stack_push(st, &ls);
		}
		if (p < rs)
		{
			stack_push(st, &rs);
			++p;
			stack_push(st, &p);
			--p;
		}
	}
}

int main() {
	int arr[] = {5, 13, 27, 99, 0, 4};
	size_t size = sizeof(arr) / sizeof(int);
	q_sort(arr, size);
	for (size_t i = 0; i < size; ++i)
		printf("%d ", arr[i]);
}
