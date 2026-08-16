#pragma once

#include <result.h>
#include <stdarg.h>
#include <stdint.h>
#include <traits.h>

Result fmt(Writer writer[const static 1], char fmt[const static 1], ...);
Result vfmt(Writer writer[const static 1], char fmt[const static 1], va_list args);

char* utoa(uint64_t value, char str[static 1], int base);
