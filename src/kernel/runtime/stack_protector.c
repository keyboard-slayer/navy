#include <arch/interface.h>
#include <logging.h>
#include <stdint.h>
#include <stdnoreturn.h>

size_t __stack_chk_guard = (size_t)__TIMESTAMP__;

noreturn void __stack_chk_fail(void) {
    error$("stack protector check fail");
    breakpoint();
}
