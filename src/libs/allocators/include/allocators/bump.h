#pragma once

#include <result.h>
#include <stdint.h>
#include <traits.h>

typedef struct {
    Allocator base;

    void* buffer;
    size_t ptr;
    size_t length;
} BumpAllocator;

BumpAllocator bump_allocator_create(void* start, size_t len);
