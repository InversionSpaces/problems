#pragma once

#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    void* ptr;
    size_t size;
} FileData;

void write_file(FileData data, const char* filename);

char* read_file_str(const char *filename);

FileData read_file_bin(const char *filename);
