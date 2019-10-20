#pragma once

#include <stdlib.h>
#include <inttypes.h>

int get_command_id(const char *name);
int get_command_id(const uint8_t hex);

const char* get_command_name(int id);
uint8_t get_command_binary(int id);

#include "binaryfile.h"
#include "cpu.h"

#define PROCESSOR_FUNC_ARGS \
(const char* args[], size_t argc, CommandsContainer* container)

#define EXECUTOR_FUNC_ARGS \
(CPU* cpu, BinCommand cmd)

int (*get_processor(int id)) PROCESSOR_FUNC_ARGS;
int (*get_executor(int id)) EXECUTOR_FUNC_ARGS;
