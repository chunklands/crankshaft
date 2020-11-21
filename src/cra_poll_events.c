#include "crankshaft_internal.h"

int
cra_poll_events (cra_engine_t engine)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_CHECK_THREAD_MAIN (engine);
        ICRA_CHECK_LOGIC (icra_crankshaft != NULL);

        glfwPollEvents ();
        return CRA_OK;
}
