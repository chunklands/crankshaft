#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_mem.h"
#include "icra_ulc_closure.h"
#include <glib/glib.h>

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
        cra_ulc_engine cb;
        void *data;
} * dd_t;

gboolean
dispatch_engine_stop (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        // TODO(daaitch): maybe not async

        icra_ulc_mainthread_enqueue_closure (dd->engine, &dd->closure, cra_ok,
                                             dd->engine);

        return G_SOURCE_REMOVE;
}

int
cra_engine_stop (cra_engine_t engine, cra_ulc_engine callback,
                 void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);

        ICRA_CHECK_LOGIC (!engine->gl_loop_shouldquit);
        engine->gl_loop_shouldquit = true;

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->engine = engine;

        icra_engine_openglthread_enqueue_dispatcher (engine,
                                                     dispatch_engine_stop, dd);
        return cra_ok;
}
