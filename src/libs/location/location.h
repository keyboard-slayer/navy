#pragma once

#include <stddef.h>

typedef struct
{
    const char* file;
    size_t fileLen;

    const char* full;
    size_t fullLen;

    const char* func;
    size_t funcLen;

    size_t col;
    size_t line;
} Loc;

#define loc$() ((Loc){                    \
    .file = __FILE_NAME__,                \
    .fileLen = sizeof(__FILE_NAME__) - 1, \
    .full = __FILE__,                     \
    .fullLen = sizeof(__FILE__) - 1,      \
    .func = __func__,                     \
    .funcLen = sizeof(__func__) - 1,      \
    .line = __LINE__,                     \
    .col = 0,                             \
})
