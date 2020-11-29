
#include "icra_ulh_closure.h"
#include "icra_checks.h"
#include "icra_mem.h"

void
icra_ulh_closure_init (cra_ulh_closure_t closure, void (*ulh) (),
                       void *ulh_uld, icra_ulh_finalizer finalizer)
{
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (ulh != NULL);
        ICRA_UNASSERTED (ulh_uld);
        ICRA_ASSERT (finalizer != NULL);

        closure->ulh = ulh;
        closure->ulh_uld = ulh_uld;
        closure->finalizer = finalizer;
}

void
icra_ulh_closure_deleter_finalizer (cra_ulh_closure_t closure)
{
        icra_free (closure);
}

void
icra_ulh_closure_noop_finalizer (cra_ulh_closure_t closure)
{
        // do nothing
}
