#include "glfw.h"
#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_log.h"
#include "icra_mem.h"
#include "icra_ulc_closure.h"
#include <glib.h>

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
} * dd_t;

static void
mainthread_enqueue_closure (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);

        ICRA_ASSERT_THREAD_OPENGL (dd->engine);
        icra_ulc_mainthread_enqueue_closure (dd->engine, &dd->closure, cra_ok,
                                             dd->engine);
}

static gboolean
dispatch_engine_stop (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();

        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->engine != NULL);
        ICRA_ASSERT (!dd->engine->gl_loop_quit);

        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        cra_engine_t engine = dd->engine;

        icra_log_info ("stop engine %p dispatched", (void *)engine);

        engine->gl_loop_quit = true;
        engine->gl_loop_quit_callback
            = (cra_callback)mainthread_enqueue_closure;
        engine->gl_loop_quit_callback_data = dd;

        return G_SOURCE_REMOVE;
}

int
cra_engine_stop (cra_engine_t engine, cra_ulc_engine callback,
                 void *callback_data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (engine);

        icra_log_info ("stop engine %p", (void *)engine);

        ICRA_ASSERT (!engine->gl_loop_shouldquit);
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
