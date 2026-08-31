#include <stddef.h>
#include <string.h>

#include "fmt.h"

static char* strrev(char str[static 1]) {
    int start;
    int end;
    char tmp;

    end = strlen(str) - 1;
    start = 0;

    while (start < end) {
        tmp = str[start];
        str[start] = str[end];
        str[end] = tmp;
        start++;
        end--;
    }
    return str;
}

static char* itoa(int64_t value, char str[static 1], int base) {
    int i = 0;
    bool isNegative = false;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    if (value < 0 && base == 10) {
        isNegative = true;
        value = -value;
    }

    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }

    if (isNegative) {
        str[i++] = '-';
    }

    str[i] = '\0';

    return strrev(str);
}

char* utoa(uint64_t value, char str[static 1], int base) {
    int i = 0;

    if (value == 0) {
        str[i++] = '0';
        str[i] = '\0';
        return str;
    }

    while (value != 0) {
        int rem = value % base;
        str[i++] = (rem > 9) ? (rem - 10) + 'a' : rem + '0';
        value = value / base;
    }

    str[i] = '\0';

    return strrev(str);
}

Result fmt(Writer writer[const static 1], char fmt[const static 1], ...) {
    va_list args;
    va_start(args, fmt);
    Result res = vfmt(writer, fmt, args);
    va_end(args);
    return res;
}

Result vfmt(Writer writer[const static 1], char _fmt[const static 1], va_list args) {
    const char* s = _fmt;

    while (*s) {
        if (*s == '%') {
            switch (*++s) {
            case 'd': {
                s++;
                char buf[100];
                intptr_t value = va_arg(args, intptr_t);

                itoa(value, buf, 10);
                writer->write(writer, strlen(buf), buf);
                break;
            }

            case 'p': {
                s++;
                char buf[100];
                size_t value = va_arg(args, size_t);

                utoa(value, buf, 16);
                writer->write(writer, 2, "0x");

                for (size_t i = strlen(buf); i < sizeof(size_t) * 2; i++) {
                    writer->write(writer, 1, "0");
                }

                writer->write(writer, strlen(buf), buf);
                break;
            }

            case 'x': {
                s++;
                char buf[100];
                size_t value = va_arg(args, size_t);

                utoa(value, buf, 16);
                writer->write(writer, strlen(buf), buf);
                break;
            }

            case 's': {
                s++;
                char* value = va_arg(args, char*);
                writer->write(writer, strlen(value), value);
                break;
            }

            case 'c': {
                s++;
                char value = va_arg(args, int);
                writer->write(writer, 1, &value);
                break;
            }

            case '%': {
                s++;
                writer->write(writer, 1, "%");
                break;
            }

            case 'r': {
                s++;
                Result res = va_arg(args, Result);

                if (res.type != EOK) {
                    char* const type = res_type_str[res.type];
                    writer->write(writer, 4, "Err(");
                    writer->write(writer, strlen(type), type);
                    writer->write(writer, 1, ")");
                } else {
                    writer->write(writer, 3, "Ok(");
                    fmt(writer, "%x", res.value);
                    writer->write(writer, 1, ")");
                }

                break;
            }

            default: {
                return Err(EINVAL);
            }
            }
        } else {
            char sub[2] = {*s++, '\0'};
            writer->write(writer, 1, sub);
        }
    }

    return Ok();
}
