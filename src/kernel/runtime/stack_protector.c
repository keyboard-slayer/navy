#include <arch/interface.h>
#include <logging.h>
#include <stdint.h>
#include <stdnoreturn.h>

uint64_t __stack_chk_guard = (uint64_t)__TIMESTAMP__;

noreturn void __stack_chk_fail(void) {
    error$("stack protector check fail");
    breakpoint();
}
