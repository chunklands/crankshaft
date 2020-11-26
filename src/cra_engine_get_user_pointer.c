#include "icra_checks.h"
#include "icra_engine.h"
#include "icra_log.h"

int
cra_engine_get_user_pointer (cra_engine_t engine, void **data_ptr)
{
        ICRA_PREAMBLE_API ();
        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_CHECK_PARAM_NOTNULL (data_ptr);

        *data_ptr = engine->uld;
        return cra_ok;
}
