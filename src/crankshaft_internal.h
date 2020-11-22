#ifndef __CRANKSHAFT_INTERNAL_H__
#define __CRANKSHAFT_INTERNAL_H__

#include "glfw.h"
#include <crankshaft.h>
#include <glib/glib.h>
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
#define ICRA_CHECK_PARAM(expr) ICRA_CHECK ((expr), CRA_ERR_PARAM)
#define ICRA_CHECK_PARAM_NULL(expr) ICRA_CHECK ((expr) == NULL, CRA_ERR_PARAM)
#define ICRA_CHECK_PARAM_NOTNULL(expr)                                        \
        ICRA_CHECK ((expr) != NULL, CRA_ERR_PARAM)

#define ICRA_CHECK_THREAD_MAIN(engine)                                        \
        ICRA_CHECK (icra_is_main_thread (engine), CRA_ERR_THREAD)

#define ICRA_CHECK_LOGIC(expr) ICRA_CHECK ((expr), CRA_ERR_LOGIC)
#define ICRA_CHECK_LIB(expr) ICRA_CHECK ((expr), CRA_ERR_LIB)

#define ICRA_DISPATCH_DATA_CLOSURE_FIELD struct cra_callback_closure_s closure;
#define ICRA_DISPATCH_DATA_AS_CLOSURE(dispatch_data)                          \
        ((cra_callback_closure_t) (dispatch_data))
#define ICRA_AS_CALLBACK(callback) ((cra_callback) (callback))

#define ICRA_DISPATCH(engine, fn, data)                                       \
        do                                                                    \
        {                                                                     \
                g_main_context_invoke (engine->context, G_SOURCE_FUNC ((fn)), \
                                       (data));                               \
        } while (0)

gboolean icra_dispatch_make_callback (cra_engine_t engine,
                                      cra_callback_closure_t closure,
                                      cra_status status, void *result);

#define ICRA_DISPATCH_MAKE_CALLBACK(engine, data, status, result)             \
        do                                                                    \
        {                                                                     \
                (data)->closure.callback_status = (status);                   \
                (data)->closure.callback_result = (result);                   \
                CraCallbackHandlerResult handler_result                       \
                    = (engine)->callback_handler (                            \
                        ICRA_DISPATCH_DATA_AS_CLOSURE ((data)),               \
                        (engine)->callback_handler_data);                     \
                if (handler_result == CRA_FREE_DATA)                          \
                        g_free ((data));                                      \
        } while (0)

#define ICRA_MALLOC(var)                                                      \
        do                                                                    \
        {                                                                     \
                var = g_malloc0 (sizeof (*var));                              \
        } while (0)
#define ICRA_FREE(var)                                                        \
        do                                                                    \
        {                                                                     \
                g_free ((var));                                               \
        } while (0)

bool icra_is_main_thread (cra_engine_t engine);
bool icra_is_opengl_thread (cra_engine_t engine);

struct icra_crankshaft_s
{
        cra_error_handler error_handler;
        void *error_handler_data;

        int glfw_refresh_rate;
};

struct icra_crankshaft_s *icra_crankshaft;

struct icra_engine_s
{
        cra_callback_handler callback_handler;
        void *callback_handler_data;

        bool initialized;
        GMainContext *context;

        GThread *main_thread;

        bool gl_loop_shouldquit;
        GMainLoop *gl_loop;
        void *gl_last_result;
        GThread *gl_thread;
        cra_callback_closure_t gl_quit_handler;

        int glfw_refresh_rate;

        GHashTable *window_instances;
};

struct icra_window_s
{
        cra_engine_t engine;
        GLFWwindow *glfw_window;
        bool glfw_loaded_gl;
};

void icra_closure_deleter_finalizer (cra_callback_closure_t closure);
void icra_closure_noop_finalizer (cra_callback_closure_t closure);

void icra_closure_init (cra_callback_closure_t closure, void (*callback) (),
                        void *callback_user_data,
                        cra_closure_finalizer finalizer);

#endif // __CRANKSHAFT_INTERNAL_H__
