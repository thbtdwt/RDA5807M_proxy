/* RDA5807M_trace
 * Brief: Traces Api.
 *
 * Evolution 31-Mar-2018   Thibaut .Creation
 *
 */
#ifndef RDA5807M_TRACE_H
#define RDA5807M_TRACE_H

#include <stdio.h>
#include <stdarg.h>
#include <RDA5807M_proxy.h>


/*
 * Brief: Print trace 
 * Param[in]: debug level
 * Param[in]: format
 */
#define RDA5807_printf(level,...) do { \
    RDA5807_printf_internal(level, __FUNCTION__,##__VA_ARGS__); \
} while (0)

void RDA5807_printf_internal(enum RDA5807_DEBUG_LEVEL level, const char* function, 
    const char* format,...) __attribute__ ((format (printf, 3, 4)));

/*
 * Brief: Print trace, followed by a buffer display.
 * Param[in]: debug level
 * Param[in]: buffer pointer
 * Param[in]: buffer size
 * Param[in]: format
 */
#define RDA5807_print_buffer(level, buffer, size,...) do { \
    RDA5807_print_buffer_internal(level, __FUNCTION__, (char*)buffer, size,##__VA_ARGS__); \
} while (0)

void RDA5807_print_buffer_internal(enum RDA5807_DEBUG_LEVEL level, const char* function,
    const char* buffer, unsigned int size,
    const char* format,...) __attribute__ ((format (printf, 5, 6)));

#endif //RDA5807M_TRACE_H