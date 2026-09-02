#pragma once

#include <result.h>
#include <stddef.h>

#define KM_SLEEP (0)
#define KM_NOSLEEP (1)
#define KM_NAME_LIMIT_LEN (32)

struct kmem_cache {
    char name[KM_NAME_LIMIT_LEN];
};

Result kmem_cache_create(char* name, size_t size, int align, void (*ctor)(void*), void (*dtor)(void*));

void* kmem_cache_alloc(struct kmem_cache* cp, int flags);

void kmem_cache_free(struct kmem_cache* cp, void* buf);

void kmem_cache_destroy(struct kmem_cache* cp);
