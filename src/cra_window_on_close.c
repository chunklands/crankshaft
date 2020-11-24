#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_mem.h"
#include "icra_ulh_closure.h"
#include "icra_window.h"

typedef icra_window_close_ulh_closure_t dd_t;

gboolean
dispatch_window_on_close (icra_window_close_ulh_closure_t dd)
{
        ICRA_ASSERT (dd != NULL);
        dd->window->on_close = g_slist_prepend (dd->window->on_close, dd);

        return G_SOURCE_REMOVE;
}

int
cra_window_on_close (cra_window_t window, cra_ulh_window_close handler,
                     void *handler_data)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_CHECK_PARAM_NOTNULL (handler);
        ICRA_UNCHECKED (handler_data);

        cra_engine_t engine = window->engine;

        icra_window_close_ulh_closure_t dd;
        ICRA_MALLOC (dd);

        icra_ulh_closure_init (&dd->closure, handler, handler_data,
                               icra_ulh_closure_deleter_finalizer);
        dd->window = window;

        icra_engine_openglthread_enqueue_dispatcher (
            engine, dispatch_window_on_close, dd);
        return cra_ok;
}