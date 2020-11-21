#include "crankshaft_internal.h"

int
cra_window_shouldclose (cra_window_t window, int *result)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_ASSERT (window->glfw_window != NULL);
        ICRA_CHECK_THREAD_MAIN (window->engine);

        if (glfwWindowShouldClose (window->glfw_window))
                *result = true;
        else
                *result = false;

        return CRA_OK;
}
