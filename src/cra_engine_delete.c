#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_mem.h"
#include "icra_preambles.h"
#include <glib/glib.h>

int
cra_engine_delete (cra_engine_t engine)
{
        ICRA_PREAMBLE_API ();
        ICRA_CHECK_PARAM_NOTNULL (engine);

        g_main_context_unref (engine->context);

        ICRA_FREE (engine);
        return cra_ok;
}
