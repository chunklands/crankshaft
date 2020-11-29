#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_log.h"
#include "icra_mem.h"
#include "icra_ulc_closure.h"
#include "icra_window.h"
#include <glib.h>

static void
swap_windows (cra_engine_t engine)
{
        // check engine is initialized
        if (!engine->window_instances)
                return;

        GHashTableIter iter;
        g_hash_table_iter_init (&iter, engine->window_instances);

        cra_window_t window;
        while (g_hash_table_iter_next (&iter, (gpointer *)&window, NULL))
        {
                ICRA_ASSERT (window != NULL);

                if (window->glfw_loaded_gl)
                        glfwSwapBuffers (window->glfw_window);
        }
}

static void
poll_events (void *result, cra_status status, void *user_data)
{
        ICRA_ASSERT (result == NULL);
        ICRA_ASSERT (status == cra_ok);
        ICRA_ASSERT (user_data == NULL);

        glfwPollEvents ();
}

static void *
opengl_thread_func (cra_engine_t engine)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (engine);

        icra_log_info ("start OpenGL thread");

        double last_update, last_render, last_poll;
        last_update = last_render = last_poll = glfwGetTime ();

        while (!engine->gl_loop_quit)
        {
                const double loop_start = glfwGetTime ();
                const double refresh_interval
                    = engine->glfw_refresh_rate == -1
                          ? 0.1
                          : 1.0 / (double)(engine->glfw_refresh_rate);

                const double render_diff = loop_start - last_render;
                // render(render_diff, loop_start);

                const double poll_diff = loop_start - last_poll;
                if (poll_diff >= 0.03)
                {
                        last_poll = loop_start;
                        icra_ulc_mainthread_enqueue_closure (
                            engine, &engine->gl_loop_poll_events_closure,
                            cra_ok, NULL);
                }

                const double swap_time = loop_start + refresh_interval * 0.75;
                for (double now = glfwGetTime ();
                     now < swap_time && !engine->gl_loop_quit;
                     now = glfwGetTime ())
                {
                        gboolean event_dispatched = g_main_context_iteration (
                            engine->context, FALSE);
                        if (!event_dispatched)
                        {
                                g_thread_yield ();
                                break;
                        }
                }

                // update(loop_start - last_update, loop_start);
                last_update = loop_start;

                if (!engine->gl_loop_shouldquit)
                        swap_windows (engine);
        }

        if (engine->gl_loop_quit_callback)
        {
                engine->gl_loop_quit_callback (
                    engine->gl_loop_quit_callback_data);
        }

        icra_log_info ("end OpenGL thread");

        return NULL;
}

int
cra_engine_new (cra_engine_t *engine_ptr, cra_ulc_invoker ulc_invoker,
                void *ulc_invoker_uld)
{
        ICRA_CHECK_PARAM_NOTNULL (engine_ptr);
        ICRA_CHECK_PARAM_NOTNULL (ulc_invoker);
        ICRA_UNCHECKED (ulc_invoker_uld);

        cra_engine_t engine;
        ICRA_MALLOC (engine);
        engine->ulc_invoker = ulc_invoker;
        engine->ulc_invoker_uld = ulc_invoker_uld;
        engine->context = g_main_context_new ();

        icra_ulc_closure_init (&engine->gl_loop_poll_events_closure,
                               poll_events, NULL,
                               icra_ulc_closure_noop_finalizer);

        engine->gl_thread = g_thread_new (
            "GL Thread", (GThreadFunc)opengl_thread_func, engine);

        icra_log_info ("new engine %p, with OpenGL thread %p", (void *)engine,
                       (void *)engine->gl_thread);

        *engine_ptr = engine;
        return cra_ok;
}
