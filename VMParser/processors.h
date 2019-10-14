#pragma once

#include <stdlib.h>

#include "commands.h"
#include "binaryfile.h"

int process_push(const char* args[], size_t argc, 
	CommandsContainer* container)
{
	if (argc != 2) {
		printf("## Error: expected one arg for push;\n");
		return 1;
	}
	
	Command cmd = {0, atoi(args[1]), 0};
	CommandsContainerAdd(container, cmd);
	
	return 0;
}

int process_pop(const char* args[], size_t argc,
	CommandsContainer* container)
{
	if (argc != 1) {
		printf("## Error: expected no args for pop;\n");
		return 1;
	}
	
	Command cmd = {1, 0, 0};
	CommandsContainerAdd(container, cmd);
	
	return 0;
}

int process_add(const char* args[], size_t argc,
	CommandsContainer* container)
{
	if (argc != 1) {
		printf("## Error: expected no args for add;\n");
		return 1;
	}
	
	Command cmd = {2, 0, 0};
	CommandsContainerAdd(container, cmd);
	
	return 0;
}

int process_mul(const char* args[], size_t argc,
	CommandsContainer* container)
{
	if (argc != 1) {
		printf("## Error: expected no args for mul;\n");
		return 1;
	}
	
	Command cmd = {3, 0, 0};
	CommandsContainerAdd(container, cmd);
	
	return 0;
}

int process_div(const char* args[], size_t argc,
	CommandsContainer* container)
{
	if (argc != 1) {
		printf("## Error: expected no args for div;\n");
		return 1;
	}
	
	Command cmd = {4, 0, 0};
	CommandsContainerAdd(container, cmd);
	
	return 0;
}
