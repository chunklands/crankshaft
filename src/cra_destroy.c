#include "icra_checks.h"
#include "icra_crankshaft.h"
#include "icra_log.h"
#include "icra_mem.h"

int
cra_destroy ()
{
        ICRA_PREAMBLE_API ();
        ICRA_CHECK_LOGIC (icra_crankshaft != NULL);

        icra_log_info ("crankshaft destroy");

        ICRA_FREE (icra_crankshaft);
        icra_crankshaft = NULL;

        return cra_ok;
}
