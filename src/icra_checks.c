#include "icra.h"

bool
icra_is_main_thread (cra_engine_t engine)
{
        ICRA_UNCHECKED (engine);
        return g_thread_self () == engine->main_thread;
}

bool
icra_is_opengl_thread (cra_engine_t engine)
{
        ICRA_UNCHECKED (engine);
        return g_thread_self () == engine->gl_thread;
}

bool
icra_is_status (cra_status status)
{
        return (cra_status_min < status) && (status < cra_status_max);
}