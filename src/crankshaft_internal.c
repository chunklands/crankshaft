#include "crankshaft_internal.h"

GHashTable *icra_engine_instances = NULL;
cra_error_handler icra_error_handler = NULL;
void *icra_error_handler_data = NULL;

void
icra_closure_finalizer (void *closure)
{
        ICRA_FREE (closure);
}

void
icra_closure_init (cra_callback_closure_t closure, void (*callback) (),
                   void *callback_user_data, void (*finalizer) (void *closure))
{
        closure->callback = callback;
        closure->callback_user_data = callback_user_data;
        closure->finalizer = finalizer;
}

bool
icra_is_main_thread (cra_engine_t engine)
{
        ICRA_UNCHECKED (engine);
        return g_thread_self () == engine->main_thread;
}

bool
icra_is_opengl_thread (cra_engine_t engine)
{
        ICRA_UNCHECKED (engine);
        return g_thread_self () == engine->gl_thread;
}

CraCallbackHandlerResult
icra_engine_make_callback (cra_engine_t engine, cra_callback_closure_t closure)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);

        return engine->callback_handler (closure,
                                         engine->callback_handler_data);
}

gboolean
icra_dispatch_make_callback (cra_engine_t engine,
                             cra_callback_closure_t closure, cra_status status,
                             void *result)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (status > _CRA_STATUS_MIN && status < _CRA_STATUS_MAX);
        ICRA_UNASSERTED (result);

        closure->callback_status = status;
        closure->callback_result = result;

        if (icra_engine_make_callback (engine, closure) == CRA_FREE_DATA)
                ICRA_FREE (closure);

        return G_SOURCE_REMOVE;
}