#include "crankshaft_internal.h"

int
cra_destroy ()
{
        ICRA_CHECK_LOGIC (icra_crankshaft != NULL);

        ICRA_FREE (icra_crankshaft);
        icra_crankshaft = NULL;

        return cra_ok;
}
