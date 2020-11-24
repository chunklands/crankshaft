#include "icra_engine.h"
#include "icra_checks.h"
#include <glib/glib.h>

void
icra_engine_openglthread_enqueue_dispatcher (
    cra_engine_t engine, gboolean (*dispatch) (/* void* dd */),
    void *dispatch_data)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (dispatch != NULL);
        ICRA_UNASSERTED (dispatch_data);

        g_main_context_invoke (engine->context, dispatch, dispatch_data);
}