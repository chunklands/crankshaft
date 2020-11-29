#ifndef __CRANKSHAFT_ICRA_CHECKS_H__
#define __CRANKSHAFT_ICRA_CHECKS_H__

#include "icra_crankshaft.h"
#include <glib.h>
#include <stdbool.h>

// ASSERT:
// - error in lib
// - DEBUG only
// - crash (no return status)
#define ICRA_ASSERT(expr) g_assert (expr)
#define ICRA_UNASSERTED(expr) (void)(expr)
#define ICRA_ASSERT_THREAD_MAIN(engine)                                       \
        ICRA_ASSERT (icra_is_main_thread (engine))
#define ICRA_ASSERT_THREAD_OPENGL(engine)                                     \
        ICRA_ASSERT (icra_is_opengl_thread (engine))

// CHECK:
// - user error
// - DEBUG + RELEASE
// - return error status
#define ICRA_CHECK(expr, val)                                                 \
        G_STMT_START                                                          \
        {                                                                     \
                if (G_LIKELY (expr))                                          \
                {                                                             \
                }                                                             \
                else                                                          \
                {                                                             \
                        g_error ("CHECK @ " G_STRLOC ": " #expr);             \
                        if (icra_crankshaft->error_handler)                   \
                        {                                                     \
                                icra_crankshaft->error_handler (              \
                                    (val),                                    \
                                    icra_crankshaft->error_handler_data);     \
                        }                                                     \
                        return (val);                                         \
                }                                                             \
        }                                                                     \
        G_STMT_END

#define ICRA_UNCHECKED(expr) (void)(expr)
#define ICRA_CHECK_PARAM(expr) ICRA_CHECK ((expr), cra_err_param)
#define ICRA_CHECK_PARAM_NULL(expr) ICRA_CHECK ((expr) == NULL, cra_err_param)
#define ICRA_CHECK_PARAM_NOTNULL(expr)                                        \
        ICRA_CHECK ((expr) != NULL, cra_err_param)

#define ICRA_CHECK_THREAD_MAIN(engine)                                        \
        ICRA_CHECK (icra_is_main_thread (engine), cra_err_thread)

#define ICRA_CHECK_LOGIC(expr) ICRA_CHECK ((expr), cra_err_logic)
#define ICRA_CHECK_LIB(expr) ICRA_CHECK ((expr), cra_err_lib)

#define ICRA_CHECK_STATUS(status) ICRA_CHECK ((status) == cra_ok, status)

bool icra_is_main_thread (cra_engine_t engine);
bool icra_is_opengl_thread (cra_engine_t engine);
bool icra_is_status (cra_status status);

#endif // __CRANKSHAFT_ICRA_CHECKS_H__
