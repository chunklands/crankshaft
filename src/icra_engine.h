#ifndef __CRANKSHAFT_ICRA_ENGINE_H__
#define __CRANKSHAFT_ICRA_ENGINE_H__

#include <crankshaft.h>
#include <glib/glib.h>
#include <stdbool.h>

struct icra_engine_s
{
        cra_ulc_invoker ulc_invoker;
        void *ulc_invoker_uld;

        bool initialized;
        GMainContext *context;

        GThread *main_thread;

        bool gl_loop_shouldquit;
        GMainLoop *gl_loop;
        void *gl_last_result;
        GThread *gl_thread;

        int glfw_refresh_rate;

        GHashTable *window_instances;
};

void icra_engine_openglthread_enqueue_dispatcher (
    cra_engine_t engine, gboolean (*dispatch) (/* void* dd */), void *dd);

#endif // __CRANKSHAFT_ICRA_ENGINE_H__