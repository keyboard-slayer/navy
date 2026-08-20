#include <Block_compat.h>
#include <allocators/freelist.h>
#include <arch/interface.h>
#include <logging.h>
#include <mm/pmm.h>

static Result block_allocator_init(FreelistAllocator self[static 1]) {
    void* ptr = (void*)try$(pmm_alloc(psize()));
    *self = freelist_allocator_create(ptr, psize());
    return ok$();
}

static FreelistAllocator alloc;

void* block_malloc(size_t n) {
    static bool is_init = false;

    if (!is_init) {
        Result init = block_allocator_init(&alloc);
        if (init.type != EOK) {
            debug$("Couldn't initialise bump allocator for blockruntime: %r", init);
            return nullptr;
        }
    }

    Result ptr = alloc.base.alloc(&alloc, n + sizeof(BlockAllocation));
    if (ptr.type == ENOMEM) {
        Result mem = pmm_alloc(psize());
        if (mem.type != EOK) {
            debug$("Couldn't allocate more memory for blockruntime: %r", mem);
            return nullptr;
        }

        freelist_allocator_refill(&alloc, (void*)mem.uvalue, psize());
        return block_malloc(n);
    } else if (ptr.type != EOK) {
        debug$("Couldn't allocate more memory for blockruntime: %r", ptr);
        return nullptr;
    }

    BlockAllocation* ret = (BlockAllocation*)ptr.uvalue;
    ret->sz = n + sizeof(BlockAllocation);

    return (void*)((uint8_t*)ret + sizeof(BlockAllocation));
}

void block_free(void* ptr) {
    BlockAllocation* block = (BlockAllocation*)((uint8_t*)ptr - sizeof(BlockAllocation));
    alloc.base.free(&alloc, ptr, block->sz);
}
