#include "include/allocators/freelist.h"

static Result alloc(void* ctx, size_t sz) {
    FreelistAllocator* self = (FreelistAllocator*)ctx;
    Freelist** node = &self->head;

    size_t aligned_sz = __builtin_align_up(sz, 8);

    while (*node != nullptr) {
        if ((*node)->size >= aligned_sz) {
            void* ret = (void*)*node;
            size_t len = (*node)->size - aligned_sz;
            Freelist* next = (*node)->next;

            if (len < sizeof(Freelist)) {
                *node = next;
            } else {
                *node = (Freelist*)((uint8_t*)ret + aligned_sz);
                (*node)->size = len;
                (*node)->next = next;
            }

            return uok$(ret);
        }

        node = &(*node)->next;
    }

    return err$(ENOMEM);
}

static void free(void* ctx, void* ptr, size_t sz) {
    FreelistAllocator* self = (FreelistAllocator*)ctx;
    size_t aligned_sz = __builtin_align_up(sz, 8);

    Freelist* prev = nullptr;
    Freelist** node = &self->head;

    while (*node != nullptr && (uintptr_t)(*node) < (uintptr_t)ptr) {
        prev = *node;
        node = &(*node)->next;
    }

    Freelist* new_node = (Freelist*)ptr;
    new_node->size = aligned_sz;
    new_node->next = *node;

    if (*node != nullptr && (uintptr_t)ptr + sz == (uintptr_t)(*node)) {
        new_node->size += (*node)->size;
        new_node->next = (*node)->next;
    }

    if (prev != nullptr && (uintptr_t)prev + prev->size == (uintptr_t)ptr) {
        prev->size += new_node->size;
        prev->next = new_node->next;
    } else {
        *node = new_node;
    }
}

FreelistAllocator freelist_allocator_create(void* start, size_t len) {
    Freelist* head = (Freelist*)start;
    head->size = len;
    head->next = nullptr;

    return (FreelistAllocator){
        .base = (Allocator){
            .alloc = alloc,
            .free = free,
        },
        .head = head,
        .tail = head
    };
}

Result freelist_allocator_refill(FreelistAllocator self[static 1], void* buffer, size_t len) {
    Freelist* node = (Freelist*)buffer;
    node->size = len;
    self->tail->next = node;
    self->tail = self->tail->next;

    return ok$();
}
