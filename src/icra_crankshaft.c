#include "icra_crankshaft.h"
#include "icra_checks.h"
#include <stdlib.h>

icra_crankshaft_t icra_crankshaft = NULL;

void
icra_crankshaft_error (int status)
{
        ICRA_ASSERT (icra_crankshaft != NULL);
        ICRA_ASSERT (icra_crankshaft->error_handler != NULL);

        icra_crankshaft->error_handler (status,
                                        icra_crankshaft->error_handler_data);
}