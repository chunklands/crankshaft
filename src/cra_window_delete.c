#include "icra.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

static gboolean
dispatch_window_delete (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();

        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);

        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        gboolean removed
            = g_hash_table_remove (engine->window_instances, window);
        ICRA_ASSERT (removed);

        // TODO(daaitch): on_close clean

        GLFWwindow *glfw_window = window->glfw_window;
        icra_free (window);

        icra_log_info ("delete window %p dispatched", (void *)window);

        icra_ulc_mainthread_enqueue_closure_pre (
            engine, &dd->closure, cra_ok, NULL,
            (cra_callback)glfwDestroyWindow, glfw_window);

        return G_SOURCE_REMOVE;
}

int
cra_window_delete (cra_window_t window, cra_ulc_void callback,
                   void *callback_data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (window);

        ICRA_ASSERT (window->engine != NULL);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->window = window;

        icra_log_info ("delete window %p", (void *)window);

        icra_engine_openglthread_enqueue_dispatcher (
            window->engine, dispatch_window_delete, dd);
        return cra_ok;
}