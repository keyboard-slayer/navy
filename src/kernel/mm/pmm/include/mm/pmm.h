#pragma once

#include <result.h>
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

static char* const memmap_type_str[] = {
#define X(e) #e,
    MEMMAP_TYPE
#undef X
};

typedef struct {
    uintptr_t addr;
    size_t length;
    enum memmap_type type;
} MemMap;

Result pmm_setup(size_t n, MemMap map[const static n]);

Result pmm_alloc(size_t sz);
