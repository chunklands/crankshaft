#include "crankshaft_internal.h"

GHashTable *icra_engine_instances = NULL;
cra_error_handler icra_error_handler = NULL;
void *icra_error_handler_data = NULL;

void
icra_closure_deleter_finalizer (cra_callback_closure_t closure)
{
        ICRA_FREE (closure);
}

void
icra_closure_noop_finalizer (cra_callback_closure_t closure)
{
        // do nothing
}

void
icra_closure_init (cra_callback_closure_t closure,
                   cra_userland_callback userland_callback,
                   void *userland_callback_userland_data,
                   cra_closure_finalizer finalizer)
{
        closure->userland_callback = userland_callback;
        closure->userland_callback_userland_data
            = userland_callback_userland_data;
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

bool
icra_is_status (cra_status status)
{
        return (cra_status_min < status) && (status < cra_status_max);
}

cra_callback_handler_result
icra_engine_make_callback (cra_engine_t engine, cra_callback_closure_t closure)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (closure->userland_callback != NULL);
        ICRA_ASSERT (closure->finalizer != NULL);
        ICRA_ASSERT (icra_is_status (closure->userland_callback_status));

        return engine->callback_handler (closure,
                                         engine->callback_handler_data);
}

void
icra_dispatch_make_callback (cra_engine_t engine,
                             cra_callback_closure_t closure,
                             cra_status userland_callback_status,
                             void *userland_callback_result)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (closure->userland_callback != NULL);
        ICRA_ASSERT (closure->finalizer != NULL);
        ICRA_ASSERT (icra_is_status (userland_callback_status));
        ICRA_UNASSERTED (userland_callback_result);

        closure->userland_callback_status = userland_callback_status;
        closure->userland_callback_result = userland_callback_result;

        if (icra_engine_make_callback (engine, closure) == cra_finalize_data)
                closure->finalizer (closure);
}

void
icra_dispatch_make_pre_callback (cra_engine_t engine,
                                 cra_callback_closure_t closure,
                                 cra_status userland_callback_status,
                                 void *userland_callback_result,
                                 cra_callback pre_userland_callback,
                                 void *pre_userland_callback_data)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (icra_is_status (userland_callback_status));
        ICRA_UNASSERTED (userland_callback_result);
        ICRA_UNASSERTED (pre_userland_callback);
        ICRA_UNASSERTED (pre_userland_callback_data);

        ICRA_ASSERT (closure->pre_userland_callback == NULL);
        ICRA_ASSERT (closure->pre_userland_callback_data == NULL);

        closure->pre_userland_callback = pre_userland_callback;
        closure->pre_userland_callback_data = pre_userland_callback_data;

        icra_dispatch_make_callback (engine, closure, userland_callback_status,
                                     userland_callback_result);
}