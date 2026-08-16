#pragma once

#include <result.h>

typedef struct {
    Result (*write)(void*, size_t n, char[const static n]);
} Writer;

typedef struct {
    Result (*alloc)(void*, size_t);
    void (*free)(void*, void*);
} Allocator;
