#include <abstract/arch.h>
#include <stdint.h>

/*
 * Don't hit me it's temporary,
 * When I'll have a DTB parser it will be
 * gone I promise UwU
 */

volatile static uint8_t *uart = (uint8_t *)0x09000000;

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