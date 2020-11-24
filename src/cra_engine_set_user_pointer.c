#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_preambles.h"

int
cra_engine_set_user_pointer (cra_engine_t engine, void *data)
{
        ICRA_PREAMBLE_API ();

        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_UNCHECKED (data);

        engine->uld = data;
        return cra_ok;
}
