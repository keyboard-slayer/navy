#pragma once

#include <mm/paging.h>
#include <mm/pmm.h>
#include <result.h>
#include <stdint.h>

#define PAGE_GET_PHYS(x) (x & 0x000ffffffffff000)
#define PMLX_GET_INDEX(addr, level) (((uint64_t)addr & ((uint64_t)0x1ff << (12 + level * 9))) >> (12 + level * 9))

struct [[gnu::packed]] _pmap {
    uintptr_t* _raw;
};

enum pml_fields : uint64_t {
    X64_PAGE_PRESENT = 1 << 0,
    X64_PAGE_WRITABLE = 1 << 1,
    X64_PAGE_USER = 1 << 2,
    X64_PAGE_WRITE_THROUGH = 1 << 3,
    X64_PAGE_NO_CACHE = 1 << 4,
    X64_PAGE_ACCESSED = 1 << 5,
    X64_PAGE_DIRTY = 1 << 6,
    X64_PAGE_HUGE = 1 << 7,
    X64_PAGE_GLOBAL = 1 << 8,
    X64_PAGE_NO_EXECUTE = (uint64_t)1 << 63,
};

Result paging_init(size_t n, MemMap map[const static n]);
