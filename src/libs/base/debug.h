#pragma once

#include <fmt/fmt.h>

#define __FILENAME__ \
    (__builtin_strrchr(__FILE__, '/') ? __builtin_strrchr(__FILE__, '/') + 1 : __FILE__)

#define print$(FORMAT, ...) \
    log_impl(__FILENAME__, __LINE__, FORMAT, PRINT_ARGS(__VA_ARGS__))

#define println$(FORMAT, ...) print$(FORMAT "\n", __VA_ARGS__)
#define panic$(FORMAT, ...) panic_impl(__FILENAME__, __LINE__, FORMAT, PRINT_ARGS(__VA_ARGS__))

void log_impl(char const *filename, size_t line_nbr, char const *format,
              FmtArgs args);

void panic_impl(char const *filename, size_t line_nbr, char const *format,
                FmtArgs args);