#include <abstract/arch.h>
#include <archs/aarch64/serial.h>
#include <base/debug.h>
#include <base/macro.h>
#include <spec/dtb.h>

noreturn extern void bootstrap(void);

void aarch64_entry(void *dtb)
{
    dtb_register(dtb);

    MaybeAddr maybe_addr = dtb_find_compatible_device("arm,pl011");
    if (maybe_addr.isJust)
    {
        serial_register(maybe_addr.value);
    }

    bootstrap();
    block_return();
}