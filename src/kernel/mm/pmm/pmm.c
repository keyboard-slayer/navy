#include <allocators/freelist.h>
#include <arch/interface.h>
#include <logging.h>

#include "include/mm/pmm.h"

static uintptr_t total_pages = 0;
static FreelistAllocator alloc;

Result pmm_setup(size_t n, MemMap map[const static n]) {
    bool isInit = false;

    for (size_t i = 0; i < n; i++) {
        MemMap* m = &map[i];
        if (m->type != MEMMAP_FREE) {
            continue;
        }

        if (m->addr == 0) {
            m->addr += psize();
        }

        void* base = (void*)(__builtin_align_up(m->addr, psize()) + hhdm());
        size_t sz = __builtin_align_down(m->length, psize());

        if (!isInit) {
            isInit = true;
            alloc = freelist_allocator_create(base, sz);
        } else {
            try$(freelist_allocator_refill(&alloc, base, sz));
        }

        total_pages += sz / psize();
    }

    return Ok();
}

Result pmm_alloc(size_t sz) {
    size_t sz_aligned = __builtin_align_up(sz, psize());
    return alloc.base.alloc(&alloc, sz_aligned);
}

void pmm_free(void* ptr, size_t sz) {
    if (!__builtin_is_aligned(sz, psize())) {
        panic$("Pointer is not page aligned");
    }

    alloc.base.free(&alloc, ptr, sz);
}
