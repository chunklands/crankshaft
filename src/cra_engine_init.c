#include "icra.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
} * dd_t;

static gboolean
dispatch_engine_init (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();

        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        icra_log_info ("initialize engine %p dispatched", (void *)dd->engine);

        icra_ulc_mainthread_enqueue_closure (dd->engine, &dd->closure, cra_ok,
                                             dd->engine);

        return G_SOURCE_REMOVE;
}

int
cra_engine_init (cra_engine_t engine, cra_ulc_engine callback,
                 void *callback_data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        ICRA_CHECK_LOGIC (!engine->initialized);
        engine->initialized = true;

        icra_log_info ("initialize engine %p", (void *)engine);

        ICRA_ASSERT (engine->main_thread == NULL);
        engine->main_thread = g_thread_self ();

        ICRA_ASSERT (engine->window_instances == NULL);
        engine->window_instances
            = g_hash_table_new (g_direct_hash, g_direct_equal);

        engine->game_blocks = g_array_new (FALSE, FALSE, sizeof (cra_block_t));

        dd_t dd;
        ICRA_MALLOC (dd);

        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->engine = engine;

        icra_engine_openglthread_enqueue_dispatcher (engine,
                                                     dispatch_engine_init, dd);
        return cra_ok;
}
