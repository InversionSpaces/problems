#pragma once

void *exiting_calloc(size_t n, size_t size)
{
	void *retval = calloc(n, size);
	if (retval == NULL) {
		printf("# ERROR: Failed to calloc memory. Exiting...\n");
		exit(2);
	}

	return retval;
}

void *exiting_malloc(size_t size)
{
	void *retval = malloc(size);
	if (retval == NULL) {
		printf("# ERROR: Failed to malloc memory. Exiting...\n");
		exit(2);
	}

	return retval;
}

void *exiting_realloc(void *ptr, size_t size)
{
	void *retval = realloc(ptr, size);
	if (retval == NULL) {
		printf("# ERROR: Failed to realloc memory. Exiting...\n");
		exit(2);
	}

	return retval;
}
