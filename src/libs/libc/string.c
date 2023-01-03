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

size_t strspn(const char *s1, const char *s2)
{
    long i = 0;

    while (s1[i] == s2[i] && s1[i] && s2[i])
    {
        i++;
    }

    return i;
}

size_t strcspn(const char *s1, const char *s2)
{
    size_t i;

    for (i = 0; i < strlen(s1); i++)
    {
        for (size_t j = 0; j < strlen(s2); j++)
        {
            if (s2[j] == s1[i])
            {
                return i;
            }
        }
    }

    return i;
}

char *strtok(char *str, const char *delim)
{
    static char *last = NULL;

    if (str == NULL)
    {
        str = last;
    }

    if (str == NULL)
    {
        return NULL;
    }

    str += strspn(str, delim);

    if (*str == '\0')
    {
        return NULL;
    }

    last = str + strcspn(str, delim);

    if (*last != '\0')
    {
        *last++ = '\0';
    }

    return str;
}