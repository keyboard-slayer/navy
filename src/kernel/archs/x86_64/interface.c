#include <arch/interface.h>
#include <stddef.h>

noreturn void breakpoint(void) {
    asm volatile("int $3");
    unreachable();
}

noreturn void abort(void) {
    asm volatile("int $1");
    unreachable();
}

size_t psize(void) {
    return 4096;
}
