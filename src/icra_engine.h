#ifndef __CRANKSHAFT_ICRA_ENGINE_H__
#define __CRANKSHAFT_ICRA_ENGINE_H__

#include "icra_ulc_closure.h"
#include <crankshaft.h>
#include <glib/glib.h>
#include <stdbool.h>

struct icra_engine_s
{
        void *uld;

        cra_ulc_invoker ulc_invoker;
        void *ulc_invoker_uld;

        bool initialized;
        GMainContext *context;

        GThread *main_thread;

        cra_callback gl_loop_quit_callback;
        void *gl_loop_quit_callback_data;
        bool gl_loop_shouldquit;
        bool gl_loop_quit;
        void *gl_last_result;
        GThread *gl_thread;
        struct icra_ulc_closure_s gl_loop_poll_events_closure;

        int glfw_refresh_rate;

        GHashTable *window_instances;
};

void icra_engine_openglthread_enqueue_dispatcher (
    cra_engine_t engine, gboolean (*dispatch) (/* void* dd */), void *dd);

#endif // __CRANKSHAFT_ICRA_ENGINE_H__