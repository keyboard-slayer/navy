#include <fmt.h>
#include <stdarg.h>

#include "logging.h"

static Writer writer = {0};

static const char* level_names[LOG_EVENT_LENGTH] = {
    [LOG_NONE] = "",
    [LOG_DEBUG] = "DEBUG",
    [LOG_INFO] = "INFO",
    [LOG_WARN] = "WARN",
    [LOG_ERROR] = "ERROR",
};

static const char* level_colors[LOG_EVENT_LENGTH] = {
    [LOG_NONE] = "",
    [LOG_DEBUG] = "\e[1;32m",
    [LOG_INFO] = "\e[1;34m",
    [LOG_WARN] = "\e[1;33m",
    [LOG_ERROR] = "\e[1;31m",
};

void logging_set_stream(Writer w) {
    writer = w;
}

void _log(LogEvent event, Loc loc, char format[const static 1], ...) {
    va_list args;
    va_start(args, format);

    if (event != LOG_NONE) {
        fmt(&writer, "%s%s\e[0m ", level_colors[event], level_names[event]);
    }

    vfmt(&writer, format, args);

    if (event != LOG_NONE) {
        fmt(&writer, "\e[2m\e[9999C\e[%dD%s:%d\e[0m", loc.fileLen + 3, loc.file, loc.line);
    }

    if (event != LOG_NONE) {
        writer.write(&writer, 1, "\n");
    }

    va_end(args);
}
