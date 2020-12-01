#ifndef __CRANKSHAFT_ICRA_H__
#define __CRANKSHAFT_ICRA_H__

// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <cglm/cglm.h>
#include <cglm/struct.h>
#include <crankshaft.h>
#include <glib.h>
#include "rxi_log.h"
#include "stb.h"

// log ////////////////////////////////////////////////////////////////////////

#define ICRA_PREAMBLE(prefix)                                                 \
        G_STMT_START { log_trace ("%s: %s", (prefix), __func__); }            \
        G_STMT_END

// F = frequent invoked api calls

#define ICRA_PREAMBLE_API() ICRA_PREAMBLE ("api")
#define ICRA_PREAMBLE_DISPATCH() ICRA_PREAMBLE ("dispatch")
#define ICRA_PREAMBLE_HANDLER() ICRA_PREAMBLE ("handler")

#define icra_log_info(...) log_info (__VA_ARGS__)
#define icra_log_error(...) log_error (__VA_ARGS__)

// mem ////////////////////////////////////////////////////////////////////////

#define ICRA_MALLOC(var) ICRA_MALLOC_N ((var), 1)

#define ICRA_MALLOC_N(var, n)                                                 \
        G_STMT_START { var = icra_malloc_n (sizeof (*(var)) * (n)); }         \
        G_STMT_END

#define ICRA_MEMSET0(var)                                                     \
        G_STMT_START { memset ((var), 0, sizeof (*(var))); }                  \
        G_STMT_END

#define ICRA_MEMDUP(dst, src, n)                                              \
        G_STMT_START                                                          \
        {                                                                     \
                G_STATIC_ASSERT (sizeof (*src) == sizeof (*dst));             \
                dst = g_memdup ((src), sizeof (*src) * (n));                  \
        }                                                                     \
        G_STMT_END

static inline void *
icra_malloc_n (unsigned long n)
{
        return g_malloc0 (n);
}

static inline void
icra_free (void *ptr)
{
        g_free (ptr);
}

static inline void
icra_constfree (const void *ptr)
{
        g_free ((void *)ptr);
}

// crankshaft /////////////////////////////////////////////////////////////////

typedef struct icra_crankshaft_s
{
        cra_error_handler error_handler;
        void *error_handler_data;

        int glfw_refresh_rate;
} * icra_crankshaft_t;

icra_crankshaft_t icra_crankshaft;

void icra_crankshaft_error (int status);

// checks /////////////////////////////////////////////////////////////////////

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

// ulc ////////////////////////////////////////////////////////////////////////

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

// ulh ////////////////////////////////////////////////////////////////////////

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

///////////////////////////////////////////////////////////////////////////////
// GL /////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// program ///////////////////////////////////////////////////////////////////

typedef struct icra_gl_program_s *icra_gl_program_t;

icra_gl_program_t icra_gl_program_new (void);
void icra_gl_program_delete (icra_gl_program_t program);

int icra_gl_program_compile_shader (icra_gl_program_t program,
                                    char const *vertex_shader_source,
                                    char const *fragment_shader_source);

// vao ///////////////////////////////////////////////////////////////////

typedef struct icra_gl_vao_s *icra_gl_vao_t;

struct icra_gl_vao_init_attrib
{
        GLint size;
        GLenum type;
};

icra_gl_vao_t icra_gl_vao_new (GLenum mode);
void icra_gl_vao_delete (icra_gl_vao_t vao);

void icra_gl_vao_init (icra_gl_vao_t vao, const void *data, GLsizei count,
                       struct icra_gl_vao_init_attrib *attribs,
                       int attrib_count);
void icra_gl_vao_render (icra_gl_vao_t const vao);

// renderquad
// ///////////////////////////////////////////////////////////////////

icra_gl_vao_t icra_gl_renderquad_new (void);

///////////////////////////////////////////////////////////////////////////////
// GameObjects ////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// engine /////////////////////////////////////////////////////////////////////

struct icra_engine_s
{
        void *uld;

        cra_ulc_invoker ulc_invoker;
        void *ulc_invoker_uld;

        bool initialized;
        GMainContext *context;

        GThread *main_thread;

        cra_callback gl_loop_quit_callback;
        void *gl_loop_quit_callback_data;
        bool gl_loop_shouldquit;
        bool gl_loop_quit;
        GThread *gl_thread;
        struct icra_ulc_closure_s gl_loop_poll_events_closure;

        int glfw_refresh_rate;

        GHashTable *window_instances;

        bool render_initialized;
        icra_gl_program_t render_gbuffer;
        icra_gl_program_t render_lighting;
        icra_gl_program_t render_selectblock;
        icra_gl_program_t render_sprite;
        icra_gl_program_t render_text;
        icra_gl_vao_t render_quad;
        mat4s render_gbuffer_proj;

        // deferred rendering

        GLuint render_dr_framebuffer;
        GLuint render_dr_renderbuffer;
        GLuint render_dr_position_texture;
        GLuint render_dr_normal_texture;
        GLuint render_dr_color_texture;

        // game objects
        GArray *game_blocks;
};

void icra_engine_openglthread_enqueue_dispatcher (
    cra_engine_t engine, gboolean (*dispatch) (/* void* dd */), void *dd);

// window /////////////////////////////////////////////////////////////////////

struct icra_window_s
{
        cra_engine_t engine;
        GLFWwindow *glfw_window;
        bool glfw_loaded_gl;

        GSList *on_close;
};

typedef struct icra_window_close_ulh_closure_s
{
        ICRA_DD_HANDLER_CLOSURE_FIELD
        cra_window_t window;
} * icra_window_close_ulh_closure_t;

cra_window_t icra_window_get_from_glfw_window (GLFWwindow *glfw_window);

// block //////////////////////////////////////////////////////////////////////

struct icra_block_s
{
        char *id;
        bool opaque;
        stbi_uc *texture;
        int texture_width;
        int texture_height;
        struct cra_block_face *faces;
        int faces_length;
};

#endif // __CRANKSHAFT_ICRA_H__