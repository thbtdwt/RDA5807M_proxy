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

enum DEBUG_LEVEL {
	DEBUG,
	INFO,
	WARNING,
	ERROR
};

void RDA5807_printf(enum DEBUG_LEVEL level,const char* format,...) __attribute__ ((format (printf, 2, 3)));

#endif //RDA5807M_TRACE_H