#include "crankshaft_internal.h"

int
cra_window_get_engine (cra_window_t window, cra_engine_t *engine)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_CHECK_PARAM_NOTNULL (engine);

        ICRA_ASSERT (window->engine != NULL);

        *engine = window->engine;
        return cra_ok;
}
