#include <unistd.h>
#include <inttypes.h>
#include <stdio.h>
#include <assert.h>

typedef uint8_t byte;

static char heap_need_init = 1;

typedef struct mem_unit mem_unit;

struct mem_unit {
	char is_free;
	size_t size;
	mem_unit* next;
};

static mem_unit* head = NULL;

#define HEAP_INIT_SIZE (sizeof(mem_unit) + 100)
#define MIN_USEABLE_SIZE (sizeof(mem_unit) + 10)

void heap_init() {
	head = sbrk(0);
	
	sbrk(HEAP_INIT_SIZE);
	
	head->is_free = 1;
	head->next = NULL;
	head->size = HEAP_INIT_SIZE - sizeof(mem_unit);
}

void* cmalloc(size_t size) {
	if (heap_need_init) {
		heap_init();
		heap_need_init = 0;
	}
	
	assert(head);
	
	mem_unit* unit;
	for (unit = head;
		unit->next;
		unit = unit->next) {
		
		if (unit->is_free &&
			unit->size >= size) {
			
			if(unit->size - size < MIN_USEABLE_SIZE) {
				unit->is_free = 0;
				return (void*)(unit + 1);
			}
			
			mem_unit* new = (mem_unit*)(((byte*)(unit + 1)) + size);
			new->size = unit->size - size - sizeof(mem_unit);
			new->next = unit->next;
			
			unit->next = new;
			unit->size = size;
			unit->is_free = 0;
			
			return (void*)(unit + 1);
		}
	}	
	
	if (unit->is_free) {
		sbrk(size - unit->size);
		
		unit->size = size;
		unit->is_free = 0;
		
		return (void*)(unit + 1);
	}
	
	mem_unit* new = sbrk(0);
	
	sbrk(sizeof(mem_unit) + size);
	
	new->size = size;
	new->next = NULL;
	new->is_free = 0;
	
	unit->next = new;
	
	return (void*)(new + 1);
}

void cfree(void* ptr) {
	mem_unit* unit = ((mem_unit*)ptr) - 1;
	
	unit->is_free = 1;
	
	for (unit = head; unit && unit->next; unit = unit->next) {
		if (unit->is_free && unit->next->is_free) {
			unit->size += sizeof(mem_unit) + unit->next->size;
			unit->next = unit->next->next;
		}
	}
}

int main() {
	for(int i = 0; i < 1000; i++) {
		char* s = (char*)cmalloc(5);
		
		s[0] = i % ('z' - 'a') + 'a';
		s[1] = (i + 1) % ('z' - 'a') + 'a';
		s[2] = (i + 2) % ('z' - 'a') + 'a';
		s[3] = (i + 3) % ('z' - 'a') + 'a';
		s[4] = '\0';
		
		printf("%d:%s\n", i, s);
		
		cfree(s);
	}
}
