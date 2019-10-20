#pragma once

#include <inttypes.h>

#pragma pack(push, 1)
struct BinCommand {
	uint8_t type;
	int32_t arg1;
	int32_t arg2;
};
#pragma pack(pop)

typedef struct BinCommand BinCommand;
