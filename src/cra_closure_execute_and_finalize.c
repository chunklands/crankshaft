#include "icra_checks.h"
#include "icra_ulc_closure.h"

int
cra_closure_execute_and_finalize (cra_ulc_closure_t closure)
{
        ICRA_CHECK_PARAM_NOTNULL (closure);

        // TODO(daaitch): check for main thread => needs engine be part of
        // closure

        icra_ulc_closure_execute (closure);

        ICRA_ASSERT (closure->finalizer != NULL);
        closure->finalizer (closure);

        return cra_ok;
}