/* RDA5807M_trace
 * Brief: Traces Api.
 *
 * Evolution 31-Mar-2018   Thibaut .Creation
 *
 */

#include <time.h>
#include "RDA5807M_trace.h"

static enum DEBUG_LEVEL level_g = WARNING;

void RDA5807_printf(enum DEBUG_LEVEL level, const char* format,...)
{
    int rc;
    char trace_buffer[4096];

    if ( level < level_g )
        return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    rc = snprintf(trace_buffer, sizeof(trace_buffer), "[%d-%02d-%02d %02d:%02d:%02d] ",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec);

    va_list args;
    va_start(args, format);
    vsnprintf(trace_buffer + rc, sizeof(trace_buffer) - rc, format, args);
    va_end(args);

    fputs(trace_buffer, ( level = ERROR ) ? stderr : stdout);
}
