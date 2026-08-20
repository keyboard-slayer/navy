#pragma once

#include <result.h>
#include <stdint.h>
#include <traits.h>

typedef struct _FreeListNode {
    size_t size;
    struct _FreeListNode* next;
} Freelist;

typedef struct {
    Allocator base;

    Freelist* head;
    Freelist* tail;
} FreelistAllocator;

FreelistAllocator freelist_allocator_create(void* start, size_t len);

Result freelist_allocator_refill(FreelistAllocator self[static 1], void* buffer, size_t len);
