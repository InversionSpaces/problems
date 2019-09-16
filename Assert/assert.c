#include <stdio.h>

#define ASSERT(cond) if (!(cond)) { printf("ASSERT FAILED: %s, FILE: %s, LINE: %d, FUNC: %s\n", #cond, __FILE__, __LINE__, __PRETTY_FUNCTION__); }

int main() {
	ASSERT(0 > 1);
}
