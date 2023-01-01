#pragma once

#define UNWRAP_MAYBE (0)

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define Maybe(T)     \
    struct           \
    {                \
        int type;    \
        bool isJust; \
        T value;     \
    }

#define Just(T, x)            \
    (T)                       \
    {                         \
        .type = UNWRAP_MAYBE, \
        .isJust = true,       \
        .value = x,           \
    }

#define Nothing(T)            \
    (T)                       \
    {                         \
        .type = UNWRAP_MAYBE, \
        .isJust = false,      \
    }

typedef Maybe(size_t) MaybeSize;
typedef Maybe(void *) MaybePtr;
typedef Maybe(int) MaybeInt;
typedef Maybe(uintptr_t) MaybeAddr;