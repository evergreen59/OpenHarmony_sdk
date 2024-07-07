#include <errno.h>
#include <stdarg.h>
#include <stdlib.h>

#include "log.h"

static const char *tag = "udev";

void log_parse_environment(void)
{
}

int log_oom_internal(const char *file, int line, const char *func)
{
    return -ENOMEM;
}

void log_assert_failed(const char *text, const char *file, int line, const char *func)
{
    abort();
}

void log_assert_failed_unreachable(const char *text, const char *file, int line, const char *func)
{
    abort();
}

int log_meta(int level, const char*file, int line, const char *func, const char *format, ...)
{
    return 0;
}

int log_metav(int level, const char*file, int line, const char *func, const char *format, va_list ap)
{
    return 0;
}

int log_internal(
                int level,
                int error,
                const char *file,
                int line,
                const char *func,
                const char *format, ...)
{
    return 0;
}

int log_open(void)
{
    return 0;
}

void log_close(void)
{
}

void log_set_target(LogTarget target)
{
}

void log_set_max_level(int level)
{
}

int log_get_max_level(void)
{
    return LOG_INFO;
}
