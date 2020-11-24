#ifndef __CRANKSHAFT_ICRA_MEM_H__
#define __CRANKSHAFT_ICRA_MEM_H__

#include <glib/glib.h>

#define ICRA_MALLOC(var)                                                      \
        G_STMT_START { var = g_malloc0 (sizeof (*(var))); }                   \
        G_STMT_END

#define ICRA_FREE(var)                                                        \
        G_STMT_START { g_free ((var)); }                                      \
        G_STMT_END

#define ICRA_MEMSET0(var)                                                     \
        G_STMT_START { memset ((var), 0, sizeof (*(var))); }                  \
        G_STMT_END

#endif // __CRANKSHAFT_ICRA_MEM_H__