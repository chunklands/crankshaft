
#include "glfw.h"
#include "icra_checks.h"
#include "icra_crankshaft.h"
#include "icra_mem.h"
#include <crankshaft.h>

int
cra_init (cra_error_handler error_handler, void *data)
{
        ICRA_UNCHECKED (error_handler);
        ICRA_UNCHECKED (data);

        ICRA_CHECK_LOGIC (icra_crankshaft == NULL);
        ICRA_MALLOC (icra_crankshaft);

        icra_crankshaft->error_handler = error_handler;
        icra_crankshaft->error_handler_data = data;

        ICRA_CHECK_LIB (glfwInit () == GLFW_TRUE);

        GLFWmonitor *monitor = glfwGetPrimaryMonitor ();
        ICRA_CHECK_LIB (monitor != NULL);

        const GLFWvidmode *vidmode = glfwGetVideoMode (monitor);
        ICRA_CHECK_LIB (vidmode != NULL);

        icra_crankshaft->glfw_refresh_rate = vidmode->refreshRate;

        return cra_ok;
}
