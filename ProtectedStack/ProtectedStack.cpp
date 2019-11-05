#include <stdio.h>

typedef int stack_el_t;

#include "ProtectedStack.hpp"

int main() {
	PStack_t stack = {};
	PStackInitMACRO(&stack, 20);

	for (int i = 0; i < 100; ++i)
		PStackPush(&stack, i);

	PStackDumpMACRO(&stack);
}
