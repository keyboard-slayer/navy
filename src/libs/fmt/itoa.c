#include <libc/string.h>

#include "itoa.h"

static void strrev(char *s)
{
    char tmp;
    size_t end = strlen(s) - 1;
    size_t start = 0;

    while (start < end)
    {
        tmp = s[start];
        s[start++] = s[end];
        s[end--] = tmp;
    }
}

char *itoa(int64_t value, char *str, uint64_t base, size_t buf_size)
{
    size_t len = 0;

    if (value)
    {
        do
        {
            const char digit = (char)(value % base);
            str[len++] = digit < 10 ? '0' + digit : 'a' + digit - 10;
            value /= base;
        } while (value && (len < buf_size));

        strrev(str);
    }
    else
    {
        *str = '0';
    }

    return str;
}