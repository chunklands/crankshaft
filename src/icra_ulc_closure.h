#ifndef __CRANKSHAFT_ICRA_ULC_CLOSURE_H__
#define __CRANKSHAFT_ICRA_ULC_CLOSURE_H__

#include <crankshaft.h>
#include <glib/glib.h>

#define ICRA_DD_CALLBACK_CLOSURE_FIELD struct icra_ulc_closure_s closure;

typedef void (*cra_ulc) (void *result, cra_status status, void *user_data);
typedef void (*icra_ulc_closure_finalizer) (cra_ulc_closure_t closure);

struct icra_ulc_closure_s
{
        cra_callback pre_ulc;
        void *pre_ulc_data;

        cra_ulc ulc;
        void *ulc_result;
        cra_status ulc_status;
        void *ulc_uld;

        icra_ulc_closure_finalizer finalizer;
};

void icra_ulc_closure_finish (cra_ulc_closure_t closure, cra_status ulc_status,
                              void *ulc_result);

void icra_ulc_closure_finish_and_execute (cra_ulc_closure_t closure,
                                          cra_status ulc_status,
                                          void *ulc_result);

void icra_ulc_mainthread_enqueue_closure (cra_engine_t engine,
                                          cra_ulc_closure_t closure,
                                          cra_status ulc_status,
                                          void *ulc_result);

void icra_ulc_mainthread_enqueue_closure_pre (
    cra_engine_t engine, cra_ulc_closure_t closure, cra_status ulc_status,
    void *ulc_result, cra_callback pre_ulc, void *pre_ulc_data);

void icra_ulc_closure_deleter_finalizer (cra_ulc_closure_t closure);
void icra_ulc_closure_noop_finalizer (cra_ulc_closure_t closure);

void icra_ulc_closure_init (cra_ulc_closure_t closure, void (*ulc) (),
                            void *ulc_uld,
                            icra_ulc_closure_finalizer finalizer);

void icra_ulc_closure_execute (cra_ulc_closure_t closure);

#endif // __CRANKSHAFT_ICRA_ULC_CLOSURE_H__