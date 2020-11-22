#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
        cra_engine_t engine;
} * dd_t;

gboolean
dispatch_engine_init (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        icra_dispatch_make_callback (dd->engine, &dd->closure, cra_ok,
                                     dd->engine);

        return G_SOURCE_REMOVE;
}

int
cra_engine_init (cra_engine_t engine, cra_userland_engine_callback callback,
                 void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        ICRA_CHECK_LOGIC (!engine->initialized);
        engine->initialized = true;

        ICRA_ASSERT (engine->main_thread == NULL);
        engine->main_thread = g_thread_self ();

        ICRA_ASSERT (engine->window_instances == NULL);
        engine->window_instances
            = g_hash_table_new (g_direct_hash, g_direct_equal);

        dd_t dd;
        ICRA_MALLOC (dd);

        icra_closure_init (&dd->closure, callback, callback_data,
                           icra_closure_deleter_finalizer);
        dd->engine = engine;

        ICRA_DISPATCH (engine, dispatch_engine_init, dd);
        return cra_ok;
}
