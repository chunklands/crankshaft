#include "glfw.h"
#include "icra_checks.h"
#include "icra_preambles.h"
#include "icra_window.h"

int
cra_window_shouldclose (cra_window_t window, int *result)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_ASSERT (window->glfw_window != NULL);
        ICRA_CHECK_THREAD_MAIN (window->engine);

        if (glfwWindowShouldClose (window->glfw_window))
                *result = true;
        else
                *result = false;

        return cra_ok;
}
