#pragma once

#include "option.h"
#include "result.h"

#ifndef __RESULT_UNWRAP_SAFE_INCLUDE__
#    error "You shouldn't include this header directly"
#endif

#define __unwrap_plain(v, l) \
    _Generic((v), Result: __unwrap_result, Option: __unwrap_option)(v, l)

#define unwrap(v) ({        \
    __typeof__(v) _v = (v); \
    __unwrap_plain(_v, #v); \
})

#define __is_unwrap_safe(v) \
    _Generic((v), Result: __is_unwrap_safe_result, Option: is_some)(v)

#define try$(EXPR) ({            \
    __typeof__(EXPR) v = (EXPR); \
    if (!__is_unwrap_safe(v))    \
        return v;                \
    __unwrap_plain(v, "");       \
})
