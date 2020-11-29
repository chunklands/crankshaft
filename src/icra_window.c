
#include "icra_window.h"
#include "icra_checks.h"

cra_window_t
icra_window_get_from_glfw_window (GLFWwindow *glfw_window)
{
        ICRA_ASSERT (glfw_window != NULL);

        cra_window_t window = glfwGetWindowUserPointer (glfw_window);
        ICRA_ASSERT (window != NULL);

        return window;
}