#pragma once

#include <location.h>
#include <traits.h>

typedef enum {
    LOG_NONE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,

    LOG_EVENT_LENGTH
} LogEvent;

#define debug$(...) _log(LOG_DEBUG, loc$(), __VA_ARGS__)
#define log$(...) _log(LOG_INFO, loc$(), __VA_ARGS__)
#define warn$(...) _log(LOG_WARN, loc$(), __VA_ARGS__)
#define error$(...) _log(LOG_ERROR, loc$(), __VA_ARGS__)
#define print$(...) _log(LOG_NONE, (Loc){}, __VA_ARGS__)

void logging_set_stream(Writer w);

void _log(LogEvent event, Loc loc, char format[const static 1], ...);
