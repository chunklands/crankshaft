#include "icra_checks.h"
#include "icra_crankshaft.h"
#include "icra_mem.h"
#include "icra_preambles.h"

int
cra_destroy ()
{
        ICRA_PREAMBLE_API ();
        ICRA_CHECK_LOGIC (icra_crankshaft != NULL);

        ICRA_FREE (icra_crankshaft);
        icra_crankshaft = NULL;

        return cra_ok;
}
