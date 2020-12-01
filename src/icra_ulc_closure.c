#include "icra.h"

void
icra_ulc_closure_deleter_finalizer (cra_ulc_closure_t closure)
{
        icra_free (closure);
}

void
icra_ulc_closure_noop_finalizer (cra_ulc_closure_t closure)
{
        // do nothing
}

void
icra_ulc_closure_init (cra_ulc_closure_t closure, cra_ulc ulc, void *ulc_uld,
                       icra_ulc_closure_finalizer finalizer)
{
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (ulc != NULL);
        ICRA_UNASSERTED (ulc_uld);
        ICRA_ASSERT (finalizer != NULL);

        closure->ulc = ulc;
        closure->ulc_uld = ulc_uld;
        closure->finalizer = finalizer;
}

cra_ulc_invocation_result
icra_engine_make_callback (cra_engine_t engine, cra_ulc_closure_t closure)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (closure->ulc != NULL);
        ICRA_ASSERT (closure->finalizer != NULL);
        ICRA_ASSERT (icra_is_status (closure->ulc_status));

        return engine->ulc_invoker (closure, engine->ulc_invoker_uld);
}

void
icra_ulc_closure_finish (cra_ulc_closure_t closure, cra_status ulc_status,
                         void *ulc_result)
{
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (closure->ulc != NULL);
        ICRA_ASSERT (closure->finalizer != NULL);
        ICRA_ASSERT (icra_is_status (ulc_status));
        ICRA_UNASSERTED (ulc_result);

        closure->ulc_status = ulc_status;
        closure->ulc_result = ulc_result;
}

void
icra_ulc_closure_finish_and_execute (cra_ulc_closure_t closure,
                                     cra_status ulc_status, void *ulc_result)
{
        icra_ulc_closure_finish (closure, ulc_status, ulc_result);
        icra_ulc_closure_execute (closure);
}

void
icra_ulc_mainthread_enqueue_closure (cra_engine_t engine,
                                     cra_ulc_closure_t closure,
                                     cra_status ulc_status, void *ulc_result)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (closure->ulc != NULL);
        ICRA_ASSERT (closure->finalizer != NULL);
        ICRA_ASSERT (icra_is_status (ulc_status));
        ICRA_UNASSERTED (ulc_result);

        closure->ulc_status = ulc_status;
        closure->ulc_result = ulc_result;

        if (icra_engine_make_callback (engine, closure) == cra_finalize_data)
                closure->finalizer (closure);
}

void
icra_ulc_mainthread_enqueue_closure_pre (
    cra_engine_t engine, cra_ulc_closure_t closure, cra_status ulc_status,
    void *ulc_result, cra_callback pre_ulc, void *pre_ulc_data)
{
        ICRA_ASSERT (engine != NULL);
        ICRA_ASSERT (closure != NULL);
        ICRA_ASSERT (icra_is_status (ulc_status));
        ICRA_UNASSERTED (ulc_result);
        ICRA_UNASSERTED (pre_ulc);
        ICRA_UNASSERTED (pre_ulc_data);

        ICRA_ASSERT (closure->pre_ulc == NULL);
        ICRA_ASSERT (closure->pre_ulc_data == NULL);

        closure->pre_ulc = pre_ulc;
        closure->pre_ulc_data = pre_ulc_data;

        icra_ulc_mainthread_enqueue_closure (engine, closure, ulc_status,
                                             ulc_result);
}

void
icra_ulc_closure_execute (cra_ulc_closure_t closure)
{
        ICRA_ASSERT (closure != NULL);

        // TODO(daaitch): assert being main thread => needs engine be part of
        // closure

        if (closure->pre_ulc)
        {
                ICRA_UNASSERTED (closure->pre_ulc_data);
                closure->pre_ulc (closure->pre_ulc_data);
        }

        ICRA_ASSERT (closure->ulc != NULL);
        ICRA_UNASSERTED (closure->ulc_result);
        ICRA_ASSERT (icra_is_status (closure->ulc_status));
        ICRA_UNASSERTED (closure->ulc_uld);

        closure->ulc (closure->ulc_result, closure->ulc_status,
                      closure->ulc_uld);
}