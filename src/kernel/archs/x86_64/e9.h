#pragma once

#include <result.h>
#include <stdint.h>
#include <traits.h>

enum E9Status {
    E9_UNKNOWN,
    E9_AVAILABLE,
    E9_UNAVAILABLE,
};

Result e9_write([[gnu::unused]] void* ctx, size_t n, char buf[const static n]);

Writer e9_writer(void);

bool e9_available(void);
