#pragma once

#include <location.h>
#include <stddef.h>

#define RES_TYPE(F) \
    F(EOK)          \
    F(ENODEV)       \
    F(EINVAL)       \
    F(ENOMEM)       \
    F(ENOENT)

#define make_enum(e) e,
#define make_str(e) #e,

enum res_type {
    RES_TYPE(make_enum)
};

static const char* res_type_str[] = {RES_TYPE(make_str)};

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
