#ifndef __CRANKSHAFT_ICRA_ENGINE_H__
#define __CRANKSHAFT_ICRA_ENGINE_H__

#include "icra_gl_program.h"
#include "icra_gl_vao.h"
#include "icra_ulc_closure.h"
#include <cglm/struct.h>
#include <crankshaft.h>
#include <glib.h>
#include <stdbool.h>

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
};

void icra_engine_openglthread_enqueue_dispatcher (
    cra_engine_t engine, gboolean (*dispatch) (/* void* dd */), void *dd);

#endif // __CRANKSHAFT_ICRA_ENGINE_H__