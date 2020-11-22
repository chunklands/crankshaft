#include "crankshaft_internal.h"

void
engine_swap_windows (cra_engine_t engine)
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

void
poll_events (void *result, cra_status status, void *user_data)
{
        ICRA_ASSERT (result == NULL);
        ICRA_ASSERT (status == CRA_OK);
        ICRA_ASSERT (user_data == NULL);

        glfwPollEvents ();
}

void *
engine_thread_func (cra_engine_t engine)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (engine);

        double last_update, last_render, last_poll;
        last_update = last_render = last_poll = glfwGetTime ();

        struct cra_callback_closure_s poll_events_closure;
        icra_closure_init (&poll_events_closure, poll_events, NULL,
                           icra_closure_noop_finalizer);

        while (!engine->gl_loop_shouldquit)
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
                        icra_dispatch_make_callback (
                            engine, &poll_events_closure, CRA_OK, NULL);
                }

                const double swap_time = loop_start + refresh_interval * 0.75;
                for (double now = glfwGetTime ();
                     now < swap_time && !engine->gl_loop_shouldquit;
                     now = glfwGetTime ())
                {
                        gboolean event_dispatched = g_main_context_iteration (
                            engine->context, FALSE);
                        if (!event_dispatched)
                                break;
                }

                // update(loop_start - last_update, loop_start);
                last_update = loop_start;

                if (!engine->gl_loop_shouldquit)
                        engine_swap_windows (engine);
        }

        return NULL;
}

int
cra_engine_new (cra_engine_t *engine_ptr,
                cra_callback_handler callback_handler,
                void *callback_handler_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine_ptr);
        ICRA_CHECK_PARAM_NOTNULL (callback_handler);
        ICRA_UNCHECKED (callback_handler_data);

        cra_engine_t engine;
        ICRA_MALLOC (engine);
        engine->callback_handler = callback_handler;
        engine->callback_handler_data = callback_handler_data;
        engine->context = g_main_context_new ();
        engine->gl_loop = g_main_loop_new (engine->context, false);
        engine->gl_thread = g_thread_new (
            "GL Thread", (GThreadFunc)engine_thread_func, engine);

        *engine_ptr = engine;
        return CRA_OK;
}
