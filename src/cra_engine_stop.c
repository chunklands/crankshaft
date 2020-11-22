#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
        cra_engine_t engine;
        cra_userland_engine_callback cb;
        void *data;
} * dd_t;

gboolean
dispatch_engine_stop (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        // TODO(daaitch): maybe not async

        icra_dispatch_make_callback (dd->engine, &dd->closure, cra_ok,
                                     dd->engine);

        return G_SOURCE_REMOVE;
}

int
cra_engine_stop (cra_engine_t engine, cra_userland_engine_callback callback,
                 void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);

        ICRA_CHECK_LOGIC (!engine->gl_loop_shouldquit);
        engine->gl_loop_shouldquit = true;

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_closure_init (&dd->closure, callback, callback_data,
                           icra_closure_deleter_finalizer);
        dd->engine = engine;

        ICRA_DISPATCH (engine, dispatch_engine_stop, dd);
        return cra_ok;
}
