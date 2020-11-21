#include "crankshaft_internal.h"

void *
engine_thread_func (cra_engine_t engine)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT_THREAD_OPENGL (engine);

        g_main_loop_run (engine->gl_loop);
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
