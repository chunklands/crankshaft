#include "icra.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

static gboolean
dispatch_window_load_gl (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();

        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        glfwMakeContextCurrent (window->glfw_window);
        glfwSwapInterval (1);

        if (gladLoadGL ((GLADloadfunc)glfwGetProcAddress) == 0)
        {
                g_error ("gladLoadGL for window %p failed", (void *)window);
                icra_ulc_mainthread_enqueue_closure (engine, &dd->closure,
                                                     cra_err_lib, window);
                return G_SOURCE_REMOVE;
        }

        icra_log_info ("load OpenGL for window %p dispatched", (void *)window);

        window->glfw_loaded_gl = true;

        icra_ulc_mainthread_enqueue_closure (engine, &dd->closure, cra_ok,
                                             window);
        return G_SOURCE_REMOVE;
}

int
cra_window_load_gl (cra_window_t window, cra_ulc_window callback,
                    void *callback_data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        icra_log_info ("load OpenGL for window %p", (void *)window);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->window = window;

        icra_engine_openglthread_enqueue_dispatcher (
            window->engine, dispatch_window_load_gl, dd);
        return cra_ok;
}