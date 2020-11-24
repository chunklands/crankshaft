#ifndef __CRANKSHAFT_ICRA_ULH_CLOSURE_H__
#define __CRANKSHAFT_ICRA_ULH_CLOSURE_H__

#include <crankshaft.h>

#define ICRA_DD_HANDLER_CLOSURE_FIELD struct icra_ulh_closure_s closure;

typedef void (*icra_ulh) (/* unknown */);
typedef void (*icra_ulh_finalizer) (cra_ulh_closure_t closure);

struct icra_ulh_closure_s
{
        icra_ulh ulh;
        void *ulh_uld;

        icra_ulh_finalizer finalizer;
};

void icra_ulh_closure_init (cra_ulh_closure_t closure, void (*ulh) (),
                            void *ulh_uld, icra_ulh_finalizer finalizer);
void icra_ulh_closure_deleter_finalizer (cra_ulh_closure_t closure);
void icra_ulh_closure_noop_finalizer (cra_ulh_closure_t closure);

#endif // __CRANKSHAFT_ICRA_ULH_CLOSURE_H__