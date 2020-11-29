#ifndef __CRANKSHAFT_ICRA_MEM_H__
#define __CRANKSHAFT_ICRA_MEM_H__

#include <glib.h>

#define ICRA_MALLOC(var)                                                      \
        G_STMT_START { var = icra_malloc_n (sizeof (*(var))); }               \
        G_STMT_END

#define ICRA_MEMSET0(var)                                                     \
        G_STMT_START { memset ((var), 0, sizeof (*(var))); }                  \
        G_STMT_END

static inline void *
icra_malloc_n (unsigned long n)
{
        return g_malloc0 (n);
}

static inline void
icra_free (void *ptr)
{
        g_free (ptr);
}

static inline void
icra_constfree (const void *ptr)
{
        g_free ((void*)ptr);
}

#endif // __CRANKSHAFT_ICRA_MEM_H__