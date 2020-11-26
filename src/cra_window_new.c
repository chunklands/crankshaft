#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_log.h"
#include "icra_mem.h"
#include "icra_ulc_closure.h"
#include "icra_window.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

static gboolean
dispatch_window_new (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();

        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);
        ICRA_ASSERT (dd->window->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        icra_log_info ("create window %p dispatched", (void *)window);

        gboolean inserted
            = g_hash_table_add (engine->window_instances, window);
        ICRA_ASSERT (inserted);

        icra_ulc_mainthread_enqueue_closure (engine, &dd->closure, cra_ok,
                                             window);
        return G_SOURCE_REMOVE;
}

static void
window_close_callback_handler (GLFWwindow *glfw_window)
{
        ICRA_PREAMBLE_HANDLER ();

        ICRA_ASSERT (glfw_window != NULL);

        cra_window_t window = glfwGetWindowUserPointer (glfw_window);
        ICRA_ASSERT (window != NULL);
        ICRA_ASSERT_THREAD_MAIN (window->engine);

        icra_log_info ("window close event handled %p", (void *)window);

        for (GSList *it = window->on_close; it != NULL; it = it->next)
        {
                icra_window_close_ulh_closure_t item
                    = (icra_window_close_ulh_closure_t)it->data;

                ICRA_ASSERT (item);
                cra_ulh_window_close handler = item->closure.ulh;
                handler (window, item->closure.ulh_uld);
        }
}

int
cra_window_new (cra_engine_t engine, const cra_window_new_params_t params,
                cra_ulc_window callback, void *callback_data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_CHECK_PARAM_NOTNULL (params);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_data);

        ICRA_CHECK_THREAD_MAIN (engine);

        ICRA_CHECK_LOGIC (engine->initialized == true);

        glfwWindowHint (GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint (GLFW_CONTEXT_VERSION_MINOR, 3);

#ifdef __APPLE__
        glfwWindowHint (GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
        glfwWindowHint (GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#endif

        GLFWwindow *glfw_window = glfwCreateWindow (
            params->width, params->height, params->title, NULL, NULL);
        ICRA_CHECK_LIB (glfw_window != NULL);

        cra_window_t window;
        ICRA_MALLOC (window);
        window->engine = engine;
        window->glfw_window = glfw_window;

        icra_log_info ("create window %p", (void *)window);

        glfwSetWindowUserPointer (glfw_window, window);
        glfwSetWindowCloseCallback (glfw_window,
                                    window_close_callback_handler);

        dd_t dd;

        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->window = window;

        icra_engine_openglthread_enqueue_dispatcher (engine,
                                                     dispatch_window_new, dd);
        return cra_ok;
}
