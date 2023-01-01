#include <base/macro.h>
#include <abstract/arch.h>

noreturn void bootstrap(void)
{
    Output serial = serial_acquire();
    serial.puts("Hello, World !", 14);
    serial_release(&serial);

    loop {}
    block_return();
}