#pragma once

#include <result.h>
#include <stdint.h>

typedef enum : uint8_t {
    PAGE_NONE = 1 << 0,
    PAGE_READ = 1 << 1,
    PAGE_WRITE = 1 << 2,
    PAGE_EXECUTE = 1 << 3,
    PAGE_USER = 1 << 4,
    PAGE_HUGE = 1 << 5,
} MapFlag;

typedef struct _pmap Pmap;

Pmap kernel_pmap(void);

Result paging_map(Pmap page, uint64_t virt, uint64_t phys, size_t len, uint8_t flags);
