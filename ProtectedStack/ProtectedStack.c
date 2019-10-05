#include <stdio.h>

typedef int stack_el_t;

#include "ProtectedStack.h"

int main() {
	PStack_t stack = {};
	PStackInitMACRO(&stack, 20);
}
