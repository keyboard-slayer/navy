#include <arch/interface.h>
#include <stddef.h>

noreturn void breakpoint(void) {
    asm volatile("int $3");
    unreachable();
}
