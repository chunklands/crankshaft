#include "icra.h"

int
cra_log_set_level (cra_loglevel level)
{
        ICRA_CHECK_PARAM (level >= cra_loglevel_trace
                          && level <= cra_loglevel_fatal);

        log_set_level ((int)level);
        return cra_ok;
}