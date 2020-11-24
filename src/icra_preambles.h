#ifndef __CRANKSHAFT_ICRA_PREAMBLES_H__
#define __CRANKSHAFT_ICRA_PREAMBLES_H__

#include <glib/glib.h>

#define ICRA_PREAMBLE(domain, prefix)                                                 \
        G_STMT_START { g_log ((domain), G_LOG_LEVEL_DEBUG, "%s: %s", (prefix), __func__); }       \
        G_STMT_END

// frequent invoked api calls
#define ICRA_PREAMBLE_API() ICRA_PREAMBLE ("N", "api")
#define ICRA_PREAMBLE_API_F() ICRA_PREAMBLE ("F", "api")
#define ICRA_PREAMBLE_DISPATCH() ICRA_PREAMBLE ("N", "dsp")
#define ICRA_PREAMBLE_HANDLER() ICRA_PREAMBLE ("N", "hnd")
#define ICRA_PREAMBLE_HANDLER_F() ICRA_PREAMBLE ("F", "hnd")

#endif // __CRANKSHAFT_ICRA_PREAMBLES_H__