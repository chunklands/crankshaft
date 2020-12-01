#include "icra.h"

int
cra_destroy ()
{
        ICRA_PREAMBLE_API ();
        ICRA_CHECK_LOGIC (icra_crankshaft != NULL);

        icra_log_info ("crankshaft destroy");

        icra_free (icra_crankshaft);
        icra_crankshaft = NULL;

        return cra_ok;
}
