#pragma once

#define __STDC_LIB_EXT1__

typedef int errno_t;

#define ERRNOS \
    X(EOK)     \
    X(EDOM)    \
    X(EILSEQ)  \
    X(ERANGE)  \
    X(EINVAL)  \
    X(ENODEV)  \
    X(ENOMEM)  \
    X(ENOENT)

enum errnos {

#define X(e) e,
    ERRNOS
#undef X
};
