#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
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
                icra_dispatch_make_callback (engine, &dd->closure, cra_err_lib,
                                             window);
                return G_SOURCE_REMOVE;
        }

        window->glfw_loaded_gl = true;

        icra_dispatch_make_callback (engine, &dd->closure, cra_ok, window);
        return G_SOURCE_REMOVE;
}

int
cra_window_load_gl (cra_window_t window, cra_userland_window_callback callback,
                    void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_closure_init (&dd->closure, callback, callback_data,
                           icra_closure_deleter_finalizer);
        dd->window = window;

        ICRA_DISPATCH (window->engine, dispatch_window_load_gl, dd);
        return cra_ok;
}