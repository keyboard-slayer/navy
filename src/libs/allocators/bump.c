#include "include/allocators/bump.h"

static Result alloc(void* ctx, size_t sz) {
    BumpAllocator* self = (BumpAllocator*)ctx;
    size_t aligned_len = __builtin_align_up(sz, 8);

    if (self->ptr == self->length) {
        return err$(ENOMEM);
    }

    void* ptr = self->buffer + self->ptr;
    self->ptr += aligned_len;
    return uok$((uintptr_t)ptr);
}

static void free(void*, void*) {}

BumpAllocator bump_allocator_create(void* start, size_t len) {
    return (BumpAllocator){
        .base = (Allocator){
            .alloc = alloc,
            .free = free,
        },
        .buffer = start,
        .ptr = 0,
        .length = len,
    };
}
