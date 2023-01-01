#include <libc/string.h>

size_t strlen(const char *s)
{
    size_t len = 0;
    while (s[len] != '\0')
    {
        len++;
    }

    return len;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char *)dest)[i] = ((char *)src)[i];
    }

    return dest;
}

void *memset(void *s, int c, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        ((char *)s)[i] = (char)c;
    }

    return s;
}

char *strrchr(char const *s, int c)
{
    size_t len = strlen(s);
    for (size_t i = len; i > 0; i--)
    {
        if (s[i] == (char)c)
        {
            return (char *)&s[i];
        }
    }

    return NULL;
}

int memcmp(const void *s1, const void *s2, size_t n)
{
    for (size_t i = 0; i < n; i++)
    {
        if (((char *)s1)[i] != ((char *)s2)[i])
        {
            return ((char *)s1)[i] - ((char *)s2)[i];
        }
    }

    return 0;
}

int strncmp(const char *s1, const char *s2, size_t n)
{
    while (n--)
    {
        if (*s1++ != *s2++)
        {
            return *s1 - *s2;
        }
    }

    return 0;
}