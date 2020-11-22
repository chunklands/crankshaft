#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

gboolean
dispatch_window_delete (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);

        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        gboolean removed
            = g_hash_table_remove (engine->window_instances, window);
        ICRA_ASSERT (removed);

        return icra_dispatch_make_callback (engine, &dd->closure, CRA_OK,
                                            window);
}

int
cra_window_delete (cra_window_t window, cra_window_cb callback,
                   void *callback_user_data)
{
        ICRA_CHECK_PARAM_NOTNULL (window);

        ICRA_ASSERT (window->engine != NULL);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_closure_init (&dd->closure, callback, callback_user_data,
                           icra_closure_deleter_finalizer);
        dd->window = window;

        ICRA_DISPATCH (window->engine, dispatch_window_delete, dd);
        return CRA_OK;
}