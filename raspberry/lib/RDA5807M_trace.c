/* RDA5807M_trace
 * Brief: Traces Api.
 *
 * Evolution 31-Mar-2018   Thibaut .Creation
 *
 */

#include <time.h>
#include "RDA5807M_trace.h"

static enum RDA5807_DEBUG_LEVEL level_g = RDA5807_WARNING;

/*
#define FORMAT_HEADER { \
    time_t t = time(NULL); \
    struct tm tm = *localtime(&t); \
    rc = snprintf(trace_buffer, sizeof(trace_buffer), "[%d-%02d-%02d %02d:%02d:%02d](%s) ", \
        tm.tm_year + 1900, \
        tm.tm_mon + 1, \
        tm.tm_mday, \
        tm.tm_hour, \
        tm.tm_min,  \
        tm.tm_sec,  \
        function);  \
    va_list args;  
    va_start(args, format); \
    rc += vsnprintf(trace_buffer + rc, sizeof(trace_buffer) - rc, format, args); \
    va_end(args); \
}
*/

/*
 * Brief: Set the debug level
 * Param[in]: debug level
 */
void RDA5807_proxy_set_debug_level(enum RDA5807_DEBUG_LEVEL level)
{
    if ( level > RDA5807_WARNING )
        level_g = RDA5807_WARNING;
    else
        level_g = level;
}

/*
 * Brief: Print trace
 * Param[in]: debug level
 * Param[in]: function name
 * Param[in]: format
 */
void RDA5807_printf_internal(enum RDA5807_DEBUG_LEVEL level, const char* function, const char* format,...)
{
    int rc;
    char trace_buffer[4096];

    if ( level < level_g )
        return;

    //FORMAT_HEADER

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    rc = snprintf(trace_buffer, sizeof(trace_buffer), "[%d-%02d-%02d %02d:%02d:%02d](%s) ",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        function);

    va_list args;
    va_start(args, format);
    rc += vsnprintf(trace_buffer + rc, sizeof(trace_buffer) - rc, format, args);
    va_end(args);

    fputs(trace_buffer, ( level == RDA5807_ERROR ) ? stderr : stdout);
}

/*
 * Brief: Print trace, followed by a buffer display.
 * Param[in]: debug level
 * Param[in]: function nam
 * Param[in]: buffer pointer
 * Param[in]: buffer size
 * Param[in]: format
 */
void RDA5807_print_buffer_internal(enum RDA5807_DEBUG_LEVEL level, const char* function, const char* buffer, unsigned int size,
    const char* format,...)
{
    int rc;
    unsigned int i;
    char trace_buffer[4096];

    if ( level < level_g )
        return;

    time_t t = time(NULL);
    struct tm tm = *localtime(&t);
    rc = snprintf(trace_buffer, sizeof(trace_buffer), "[%d-%02d-%02d %02d:%02d:%02d](%s) ",
        tm.tm_year + 1900,
        tm.tm_mon + 1,
        tm.tm_mday,
        tm.tm_hour,
        tm.tm_min,
        tm.tm_sec,
        function);

    va_list args;
    va_start(args, format);
    rc += vsnprintf(trace_buffer + rc, sizeof(trace_buffer) - rc, format, args);
    va_end(args);

    for ( i = 0; i < size; i++ )
    {
       rc += snprintf(trace_buffer + rc, sizeof(trace_buffer) - rc, "\t[%02d] [0x%02x] [%c]\n",
       i, buffer[i], (buffer[i] > 0x20) ? buffer[i] : ' ') ; 
    }

    fputs(trace_buffer, ( level == RDA5807_ERROR ) ? stderr : stdout);
}