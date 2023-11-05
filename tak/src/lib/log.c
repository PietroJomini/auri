#include "log.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

const char *tak_log_leveln[4] = {"ERROR", "WARN", "INFO", "TRACE"};
const char *tak_log_levelc[4] = {"\x1b[31m", "\x1b[33m", "\x1b[32m", "\x1b[36m"};

void tak_log(const int level, const char *file, int line, const char *format, ...) {
#ifdef TAK_LOGF
    FILE *out = TAK_LOGF;
#else
    FILE *out = level <= 2 ? stderr : stdout;
#endif

    // collect time info
    time_t time_current = time(NULL);
    struct tm *time_info = localtime(&time_current);
    char time_fmt[16];
    strftime(time_fmt, 16, "%H:%M:%S", time_info);

    // log pre
    fprintf(out, "%s %s%-5s\x1b[0m %s:%d: ", time_fmt,
            tak_log_levelc[level - 1], tak_log_leveln[level - 1], file, line);

    // log message
    va_list args;
    va_start(args, format);
    vfprintf(out, format, args);
    va_end(args);

    // log post
    fprintf(out, "\n");
}