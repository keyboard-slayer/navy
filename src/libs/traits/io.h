#pragma once

#include <base/maybe.h>

typedef struct
{
    void (*putc)(char c);
    MaybeSize (*puts)(char const *s, size_t n);
} Output;