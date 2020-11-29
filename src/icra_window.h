#ifndef __CRANKSHAFT_ICRA_WINDOW_H__
#define __CRANKSHAFT_ICRA_WINDOW_H__

#include "glfw.h"
#include "icra_ulh_closure.h"
#include <crankshaft.h>
#include <glib.h>
#include <stdbool.h>

struct icra_window_s
{
        cra_engine_t engine;
        GLFWwindow *glfw_window;
        bool glfw_loaded_gl;

        GSList *on_close;
};

typedef struct icra_window_close_ulh_closure_s
{
        ICRA_DD_HANDLER_CLOSURE_FIELD
        cra_window_t window;
} * icra_window_close_ulh_closure_t;

cra_window_t icra_window_get_from_glfw_window (GLFWwindow *glfw_window);

#endif // __CRANKSHAFT_ICRA_WINDOW_H__