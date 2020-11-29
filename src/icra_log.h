#ifndef __CRANKSHAFT_ICRA_PREAMBLES_H__
#define __CRANKSHAFT_ICRA_PREAMBLES_H__

#include "rxi_log.h"
#include <glib.h>

#define ICRA_PREAMBLE(prefix)                                                 \
        G_STMT_START { log_trace ("%s: %s", (prefix), __func__); }            \
        G_STMT_END

// F = frequent invoked api calls

#define ICRA_PREAMBLE_API() ICRA_PREAMBLE ("api")
#define ICRA_PREAMBLE_DISPATCH() ICRA_PREAMBLE ("dispatch")
#define ICRA_PREAMBLE_HANDLER() ICRA_PREAMBLE ("handler")

#define icra_log_info(...) log_info (__VA_ARGS__)
#define icra_log_error(...) log_error (__VA_ARGS__)

#endif // __CRANKSHAFT_ICRA_PREAMBLES_H__
