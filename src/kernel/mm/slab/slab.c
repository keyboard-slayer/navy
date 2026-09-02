#include <string.h>

#include "include/slab.h"

Result kmem_cache_create(char* name, size_t size, int align, void (*ctor)(void*), void (*dtor)(void*)) {
    if (!name) {
        return Err(EINVAL);
    }

    return Ok();
}
