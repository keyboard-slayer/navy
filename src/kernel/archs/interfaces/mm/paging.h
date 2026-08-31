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

typedef struct {
    int width;
    void* ptr;
} Pmap;

static inline uint64_t pmap_read(Pmap self[const static 1], size_t index) {
    if (self->width == 64) {
        return ((uint64_t*)self->ptr)[index];
    } else {
        return ((uint32_t*)self->ptr)[index];
    }
}

static inline void pmap_write(Pmap self[static 1], size_t index, uint64_t value) {
    if (self->width == 64) {
        ((uint64_t*)self->ptr)[index] = value;
    } else {
        ((uint32_t*)self->ptr)[index] = (uint32_t)value;
    }
}

Pmap kernel_pmap(void);

Result paging_map(Pmap page, uintptr_t virt, uintptr_t phys, size_t len, uint8_t flags);

void paging_load(Pmap page);
