#pragma once

#include <stddef.h>
#include <stdint.h>

#define MEMMAP_TYPE       \
    X(MEMMAP_FREE)        \
    X(MEMMAP_RESERVED)    \
    X(MEMMAP_RECLAIMABLE) \
    X(MEMMAP_MODULE)

enum memmap_type {

#define X(e) e,
    MEMMAP_TYPE
#undef X

};

typedef struct {
    uint64_t addr;
    size_t length;
    enum memmap_type type;
} MemMap;

void pmm_setup(size_t n, MemMap map[const static n]);
