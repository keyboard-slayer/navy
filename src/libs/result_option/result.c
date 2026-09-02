#include <logging.h>

#include "result.h"

const char* res_to_str(Result res) {
    return res_type_str[res.type];
}

bool is_ok(Result v) {
    return v.type == EOK;
}

intptr_t __unwrap_result(Result r, const char* line) {
    if (r.type != EOK) {
        panic$("Unable to unwrap value '%r' from %s", r, line);
    }

    return r.value;
}

Option result_ok(Result v) {
    if (is_ok(v)) {
        return Some(v.value);
    } else {
        return None;
    }
}
