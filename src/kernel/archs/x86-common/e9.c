#include <logging.h>

#include "e9.h"
#include "port.h"

static enum E9Status status = E9_UNKNOWN;

Result e9_write([[gnu::unused]] void* ctx, size_t n, char buf[const static n]) {
    if (status == E9_UNKNOWN) {
        if (in8(0xe9) == 0xe9) {
            status = E9_AVAILABLE;
        } else {
            status = E9_UNAVAILABLE;
        }
    }

    if (status == E9_UNAVAILABLE) {
        return err$(ENODEV);
    }

    size_t i;
    for (i = 0; i < n; i++) {
        out8(0xe9, buf[i]);
    }

    return uok$(i);
}

Writer e9_writer(void) {
    return (Writer){.write = e9_write};
}

bool e9_available(void) {
    if (status == E9_UNKNOWN) {
        e9_write(nullptr, 1, "\n");
    }

    return status == E9_AVAILABLE;
}
