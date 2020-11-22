#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
        cra_engine_t engine;
        cra_engine_cb cb;
        void *data;
} * dd_t;

gboolean
dispatch_engine_stop (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        ICRA_ASSERT (g_main_loop_is_running (dd->engine->gl_loop));
        g_main_loop_quit (dd->engine->gl_loop);

        return icra_dispatch_make_callback (dd->engine, &dd->closure, CRA_OK,
                                            dd->engine);
}

int
cra_engine_stop (cra_engine_t engine, cra_engine_cb callback,
                 void *callback_user_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);

        ICRA_CHECK_LOGIC (!engine->gl_loop_shouldquit);
        engine->gl_loop_shouldquit = true;

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_closure_init (&dd->closure, callback, callback_user_data,
                           icra_closure_deleter_finalizer);
        dd->engine = engine;

        ICRA_DISPATCH (engine, dispatch_engine_stop, dd);
        return CRA_OK;
}
