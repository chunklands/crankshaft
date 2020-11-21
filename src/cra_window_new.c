#include "crankshaft_internal.h"

typedef struct dd_s
{
        ICRA_DISPATCH_DATA_CLOSURE_FIELD
        cra_window_t window;
} * dd_t;

gboolean
dispatch_window_new (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (dd->window != NULL);
        ICRA_ASSERT (dd->window->engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->window->engine);

        cra_window_t window = dd->window;
        cra_engine_t engine = window->engine;

        gboolean inserted
            = g_hash_table_add (engine->window_instances, window);
        ICRA_ASSERT (inserted);

        return icra_dispatch_make_callback (engine, &dd->closure, CRA_OK,
                                            window);
}

int
cra_window_new (cra_engine_t engine, const cra_window_new_params_t params,
                cra_window_cb callback, void *callback_user_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_CHECK_PARAM_NOTNULL (params);
        ICRA_UNCHECKED (callback);
        ICRA_UNCHECKED (callback_user_data);

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

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_closure_init (&dd->closure, callback, callback_user_data,
                           icra_closure_finalizer);
        dd->window = window;

        ICRA_DISPATCH (engine, dispatch_window_new, dd);
        return CRA_OK;
}
