#include <abstract/arch.h>
#include <stdint.h>

static uint8_t *uart;

void putc(char c)
{
    *uart = c;
}

MaybeSize puts(char const *s, size_t n)
{
    size_t i;
    for (i = 0; (i < n || s[i] != '\0'); i++)
    {
        putc(s[i]);
    }

    return Just(MaybeSize, i);
}

Output serial_acquire(void)
{
    // TODO: add a spinlock
    return (Output){
        .putc = putc,
        .puts = puts,
    };
}

void serial_release(Output *output)
{
    output->putc = NULL;
    output->putc = NULL;
}

void serial_register(uintptr_t addr)
{
    uart = (uint8_t *)addr;
}