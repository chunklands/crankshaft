#include "crankshaft_internal.h"

int
cra_engine_delete (cra_engine_t engine)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);

        ICRA_CHECK_LOGIC (!g_main_loop_is_running (engine->gl_loop));

        g_main_loop_unref (engine->gl_loop);
        g_main_context_unref (engine->context);

        ICRA_FREE (engine);
        return CRA_OK;
}
