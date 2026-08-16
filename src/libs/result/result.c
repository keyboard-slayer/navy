#include "result.h"

const char* res_to_str(Result res) {
    return res_type_str[res.type];
}
