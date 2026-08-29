#include <arch/interface.h>

#include "acpi/rsdt.h"

Result rsdt_load(Rsdp rsdp[static const 1]) {
    uintptr_t addr = rsdp->revision == 2 ? ((Xsdp*)rsdp)->xsdtAddress : rsdp->rsdtAddress;
    char* signature = rsdp->revision == 2 ? "XSDT" : "RSDT";

    Rsdt* self = (Rsdt*)(addr + hhdm());

    if (!acpi_validate(&self->header, signature)) {
        return err$(EINVAL);
    }

    return uok$(self);
}
