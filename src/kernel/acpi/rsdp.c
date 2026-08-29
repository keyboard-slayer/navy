#include <logging.h>
#include <string.h>

#include "acpi/rsdp.h"

static bool rsdp_checksum(size_t len, uint8_t table[static const len]) {
    uint8_t sum = 0;
    for (size_t i = 0; i < len; i++) {
        sum += table[i];
    }

    return sum == 0;
}

Result rsdp_load(uintptr_t addr) {
    Rsdp* self = (Rsdp*)addr;

    if (memcmp(self->signature, "RSD PTR ", 8) != 0 ||
        !rsdp_checksum(sizeof(Rsdp), (uint8_t*)self)) {
        return err$(EINVAL);
    }

    if (self->revision == 2) {
        debug$("XSDP is available");
        Xsdp* xsdp = (Xsdp*)self;
        if (!rsdp_checksum(xsdp->length, (uint8_t*)xsdp)) {
            return err$(EINVAL);
        }
    }

    return uok$(self);
}
