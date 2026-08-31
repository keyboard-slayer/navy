#pragma once

#include <location.h>
#include <misc/macros.h>
#include <stddef.h>
#include <stdint.h>

#define RES_TYPE \
    X(EOK)       \
    X(ENODEV)    \
    X(EINVAL)    \
    X(ENOMEM)    \
    X(ENOENT)

enum res_type {

#define X(e) e,
    RES_TYPE
#undef X

};

static char* const res_type_str[] = {
#define X(e) #e,
    RES_TYPE
#undef X
};

typedef struct
{
    enum res_type type;
    Loc loc;
    intptr_t value;
} Result;

#define __Ok0() \
    (Result) { .type = EOK, .value = 0, .loc = loc$() }

#define __Ok1(v) \
    (Result) { .type = EOK, .value = (size_t)(v), .loc = loc$() }

#define __Ok_impl(n, ...) __Ok##n(__VA_ARGS__)
#define __Ok(n, ...) __Ok_impl(n, __VA_ARGS__)
#define Ok(...) __Ok(GET_ARG_COUNT(__VA_ARGS__), __VA_ARGS__)

#define Err(t) \
    (Result) { .type = (t), .value = 0, .loc = loc$() }

intptr_t __unwrap_result(Result r, const char* line);

bool __is_unwrap_safe_result(Result v);

#define __RESULT_UNWRAP_SAFE_INCLUDE__
#include "utils.h"
#undef __RESULT_UNWRAP_SAFE_INCLUDE__
