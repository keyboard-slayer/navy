#pragma once

#include <stddef.h>

int strncmp(const char *s1, const char *s2, size_t n);
size_t strlen(char const *str);
void *memcpy(void *dest, void const *src, size_t n);
void *memset(void *s, int c, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
char *strrchr(char const *s, int c);
size_t strspn(const char *s1, const char *s2);
size_t strcspn(const char *s1, const char *s2);
char *strtok(char *str, const char *delim);