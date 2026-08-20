#pragma once

#include <stddef.h>

typedef struct {
    size_t sz;
} BlockAllocation;

void* block_malloc(size_t n);

void block_free(void* ptr);
