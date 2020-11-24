#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_mem.h"
#include "icra_ulc_closure.h"
#include "icra_window.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

gboolean
dispatch_window_load_gl (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        glfwMakeContextCurrent (window->glfw_window);
        glfwSwapInterval (1);

        if (gladLoadGL ((GLADloadfunc)glfwGetProcAddress) == 0)
        {
                icra_ulc_mainthread_enqueue_closure (engine, &dd->closure,
                                                     cra_err_lib, window);
                return G_SOURCE_REMOVE;
        }

        window->glfw_loaded_gl = true;

        icra_ulc_mainthread_enqueue_closure (engine, &dd->closure, cra_ok,
                                             window);
        return G_SOURCE_REMOVE;
}

int
cra_window_load_gl (cra_window_t window, cra_ulc_window callback,
                    void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->window = window;

        icra_engine_openglthread_enqueue_dispatcher (
            window->engine, dispatch_window_load_gl, dd);
        return cra_ok;
}