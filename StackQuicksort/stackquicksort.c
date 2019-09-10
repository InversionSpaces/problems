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

void swap(void* a, void* b, void* t, size_t s)
{
	memcpy(t, a, s);
	memcpy(a, b, s);
	memcpy(b, t, s);
}

void q_sort(void* arr, size_t n, size_t size, int (*cmp)(const void*, const void*)) {
	stack* st = stack_init(sizeof(void*));
	void* t = malloc(size);
	
	void* l = arr;
	void* r = arr + size * (n - 1);
	
	void* ls = NULL;
	void* rs = NULL;
	void* p = NULL;
	
	stack_push(st, &r);
	stack_push(st, &l);
	
	while (!stack_is_empty(st)) 
	{		
		stack_pop(st, &l);
		stack_pop(st, &r);
		
		size_t tsize = (r - l) / size + 1;
		
		if (tsize < 2) 
			continue;
		
		ls = l;
		rs = r;
		p = l + (tsize / 2) * size;
		
		while (l < r) {
			while (cmp(p, l) > 0 && l < p)
				l += size;
			while (cmp(p, r) <= 0 && r > p)
				r -= size;
				
			if (l == r) break;
		
			swap(l, r, t, size);
			
			if (p == l)
				p = r;
			else if (p == r)
				p = l;
		}
		
		if (p > ls)
		{
			p -= size;
			stack_push(st, &p);
			p += size;
			stack_push(st, &ls);
		}
		if (p < rs)
		{
			stack_push(st, &rs);
			p += size;
			stack_push(st, &p);
			p -= size;
		}
	}
	
	stack_deinit(st);
	free(t);
}

int icmp(const void* p1, const void* p2)
{
	return *(int*)p1 - *(int*)p2;
}

int main() {
	int arr[] = {5, 13, 27, 99, 0, 4, 55, 109};
	size_t size = sizeof(arr) / sizeof(int);
	q_sort(arr, size, sizeof(int), icmp);
	for (size_t i = 0; i < size; ++i)
		printf("%d ", arr[i]);
}
