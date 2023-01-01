#include <abstract/arch.h>
#include <base/macro.h>

#include "debug.h"

void log_impl(char const *filename, size_t line_nbr, char const *format,
              FmtArgs args)
{
    Output serial = serial_acquire();

    fmt$(&serial, "{}:{} ", filename, line_nbr);

    fmt_impl(&serial, format, args);
    serial_release(&serial);
}

void panic_impl(char const *filename, size_t line_nbr, char const *format,
                FmtArgs args)
{
    Output serial = serial_acquire();
    fmt$(&serial, "\033[31m{}:{}\033[0m ", filename, line_nbr);
    fmt_impl(&serial, format, args);
    serial_release(&serial);

    loop {}
    // arch_raise();
}