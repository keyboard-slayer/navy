#include <logging.h>

#include "option.h"

bool is_some(Option v) {
    return v.state == OPT_SOME;
}

intptr_t __unwrap_option(Option o, const char* line) {
    if (o.state == OPT_NONE) {
        panic$("Unable to unwrap None from %s", line);
    }
    return o.value;
}
