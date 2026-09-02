#pragma once

#include <errno.h>
#include <location.h>
#include <misc/macros.h>
#include <stddef.h>
#include <stdint.h>

#include "option.h"

static char* const res_type_str[] = {
#define X(e) #e,
    ERRNOS
#undef X
};

typedef struct
{
    enum errnos type;
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

bool is_ok(Result v);

#define is_err(v) !is_ok((v))

Option result_ok(Result v);

#define __RESULT_UNWRAP_SAFE_INCLUDE__
#include "utils.h"
#undef __RESULT_UNWRAP_SAFE_INCLUDE__
