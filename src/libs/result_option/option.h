#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    enum {
        OPT_NONE,
        OPT_SOME
    } state;

    intptr_t value;
} Option;

bool is_some(Option v);

#define is_none(v) !is_some(v)

intptr_t __unwrap_option(Option o, const char* line);

#define None \
    (Option) { .state = OPT_NONE }

#define Some(v) ((Option){.state = OPT_SOME, \
                          .value = (intptr_t)(v)})
