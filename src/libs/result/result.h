#pragma once

#include <location.h>
#include <stddef.h>

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

    union {
        size_t uvalue;
        ptrdiff_t ivalue;
    };
} Result;

#define ok$() \
    (Result) { .type = EOK, .uvalue = 0, .loc = loc$() }

#define uok$(u) \
    (Result) { .type = EOK, .uvalue = (u), .loc = loc$() }

#define iok$(i) \
    (Result) { .type = EOK, .ivalue = (i), .loc = loc$() }

#define err$(t) \
    (Result) { .type = (t), .uvalue = 0, .loc = loc$() }

#define unwrap$(v, t) \
    (t)((v).uvalue)

#define try$(EXPR)                        \
    ({                                    \
        Result __result = (Result)(EXPR); \
        if (__result.type != EOK)         \
            return __result;              \
        __result.ivalue;                  \
    })
