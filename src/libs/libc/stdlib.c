#include <stdbool.h>

#include "ctype.h"

long long strtoll(const char *nptr, char **endptr, int base)
{
    bool negative = false;
    long long result = 0;

    if (*nptr == '-')
    {
        negative = true;
        nptr++;
    }

    while (*nptr)
    {
        if (*nptr >= '0' && *nptr <= '9')
        {
            result *= base;
            result += *nptr - '0';
        }
        else if (*nptr >= 'a' && *nptr <= 'z')
        {
            result *= base;
            result += *nptr - 'a' + 10;
        }
        else if (*nptr >= 'A' && *nptr <= 'Z')
        {
            result *= base;
            result += *nptr - 'A' + 10;
        }
        else
        {
            break;
        }

        nptr++;
    }

    if (endptr)
    {
        *endptr = (char *)nptr;
    }

    return negative ? -result : result;
}