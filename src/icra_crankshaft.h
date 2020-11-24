#ifndef __CRANKSHAFT_ICRA_CRANKSHAFT_H__
#define __CRANKSHAFT_ICRA_CRANKSHAFT_H__

#include <crankshaft.h>

typedef struct icra_crankshaft_s
{
        cra_error_handler error_handler;
        void *error_handler_data;

        int glfw_refresh_rate;
} * icra_crankshaft_t;

icra_crankshaft_t icra_crankshaft;

#endif // __CRANKSHAFT_ICRA_CRANKSHAFT_H__