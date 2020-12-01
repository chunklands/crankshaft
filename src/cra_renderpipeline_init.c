#include "icra.h"

typedef struct dd_framebuffer_size_s
{
        cra_engine_t engine;
        int framebuffer_width;
        int framebuffer_height;
} * dd_framebuffer_size_t;

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
        cra_window_t window;
        struct cra_renderpipeline_init_params params;
} * dd_t;

static void
deferred_rendering_reset (cra_engine_t engine)
{
        ICRA_ASSERT (engine != NULL);

        if (engine->render_dr_position_texture != 0)
        {
                glDeleteTextures (1, &engine->render_dr_position_texture);
                engine->render_dr_position_texture = 0;
        }

        if (engine->render_dr_normal_texture != 0)
        {
                glDeleteTextures (1, &engine->render_dr_normal_texture);
                engine->render_dr_normal_texture = 0;
        }

        if (engine->render_dr_color_texture != 0)
        {
                glDeleteTextures (1, &engine->render_dr_color_texture);
                engine->render_dr_color_texture = 0;
        }

        if (engine->render_dr_renderbuffer != 0)
        {
                glDeleteRenderbuffers (1, &engine->render_dr_renderbuffer);
                engine->render_dr_renderbuffer = 0;
        }

        if (engine->render_dr_framebuffer != 0)
        {
                glDeleteFramebuffers (1, &engine->render_dr_framebuffer);
                engine->render_dr_framebuffer = 0;
        }
}

static void
framebuffer_create_texture (GLuint *texture, GLenum internalformat,
                            GLenum format, GLenum type, int width, int height)
{
        glGenTextures (1, texture);
        glBindTexture (GL_TEXTURE_2D, *texture);
        glTexImage2D (GL_TEXTURE_2D, 0, internalformat, width, height, 0,
                      format, type, NULL);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glFramebufferTexture2D (GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2,
                                GL_TEXTURE_2D, *texture, 0);
}

static gboolean
dispatch_framebuffer_size_handler (dd_framebuffer_size_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT_THREAD_OPENGL (dd->engine);

        glViewport (0, 0, dd->framebuffer_width, dd->framebuffer_height);
        float aspect
            = (float)dd->framebuffer_width / (float)dd->framebuffer_height;
        glm_perspective (glm_rad (75.f), aspect, 0.1f, 1000.f,
                         dd->engine->render_gbuffer_proj.raw);

        // TODO(daaitch): sprite proj
        // TODO(daaitch): text proj

        cra_engine_t engine = dd->engine;

        deferred_rendering_reset (engine);
        glGenFramebuffers (1, &engine->render_dr_framebuffer);
        glBindFramebuffer (GL_FRAMEBUFFER, engine->render_dr_framebuffer);

        ICRA_ASSERT (engine->render_dr_position_texture == 0);
        ICRA_ASSERT (engine->render_dr_normal_texture == 0);
        ICRA_ASSERT (engine->render_dr_color_texture == 0);

        framebuffer_create_texture (
            &engine->render_dr_position_texture, GL_RGB16F, GL_RGB, GL_FLOAT,
            dd->framebuffer_width, dd->framebuffer_height);
        framebuffer_create_texture (
            &engine->render_dr_normal_texture, GL_RGB16F, GL_RGB, GL_FLOAT,
            dd->framebuffer_width, dd->framebuffer_height);
        framebuffer_create_texture (
            &engine->render_dr_color_texture, GL_RGBA, GL_RGBA,
            GL_UNSIGNED_BYTE, dd->framebuffer_width, dd->framebuffer_height);

        const GLuint attachments[3] = {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
        };

        glDrawBuffers (3, attachments);
        glGenRenderbuffers (1, &engine->render_dr_renderbuffer);
        glBindRenderbuffer (GL_RENDERBUFFER, engine->render_dr_renderbuffer);
        glRenderbufferStorage (GL_RENDERBUFFER, GL_DEPTH_COMPONENT,
                               dd->framebuffer_width, dd->framebuffer_height);
        glFramebufferRenderbuffer (GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                                   GL_RENDERBUFFER,
                                   engine->render_dr_renderbuffer);

        if (glCheckFramebufferStatus (GL_FRAMEBUFFER)
            != GL_FRAMEBUFFER_COMPLETE)
        {
                icra_crankshaft_error (cra_err_gl);
                deferred_rendering_reset (engine);
                return G_SOURCE_REMOVE;
        }

        glEnable (GL_DEPTH_TEST);
        glEnable (GL_CULL_FACE);
        glCullFace (GL_BACK);
        glFrontFace (GL_CW);

        return G_SOURCE_REMOVE;
}

static void
framebuffer_size_handler (GLFWwindow *glfw_window, int width, int height)
{
        ICRA_ASSERT (glfw_window != NULL);
        cra_window_t window = icra_window_get_from_glfw_window (glfw_window);

        ICRA_ASSERT_THREAD_MAIN (window->engine);

        dd_framebuffer_size_t dd;
        ICRA_MALLOC (dd);
        dd->engine = window->engine;
        dd->framebuffer_width = width;
        dd->framebuffer_height = height;

        icra_engine_openglthread_enqueue_dispatcher (
            window->engine, dispatch_framebuffer_size_handler, dd);
}

gboolean
dispatch_renderpipeline_init (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_ASSERT (!dd->engine->render_initialized);

        cra_engine_t engine = dd->engine;
        cra_window_t window = dd->window;
        int status = cra_ok;

        icra_gl_program_t gbuffer_program = icra_gl_program_new ();
        icra_gl_program_t lighting_program = icra_gl_program_new ();
        icra_gl_program_t selectblock_program = icra_gl_program_new ();
        icra_gl_program_t sprite_program = icra_gl_program_new ();
        icra_gl_program_t text_program = icra_gl_program_new ();

        status = icra_gl_program_compile_shader (
            gbuffer_program, dd->params.gbuffer_vertex_shader,
            dd->params.gbuffer_fragment_shader);
        if (status != cra_ok)
                goto failfast;

        status = icra_gl_program_compile_shader (
            lighting_program, dd->params.lighting_vertex_shader,
            dd->params.lighting_fragment_shader);
        if (status != cra_ok)
                goto failfast;

        status = icra_gl_program_compile_shader (
            selectblock_program, dd->params.selectblock_vertex_shader,
            dd->params.selectblock_fragment_shader);
        if (status != cra_ok)
                goto failfast;

        status = icra_gl_program_compile_shader (
            sprite_program, dd->params.sprite_vertex_shader,
            dd->params.sprite_fragment_shader);
        if (status != cra_ok)
                goto failfast;

        status = icra_gl_program_compile_shader (
            text_program, dd->params.text_vertex_shader,
            dd->params.text_fragment_shader);
        if (status != cra_ok)
                goto failfast;

        engine->render_initialized = true;
        engine->render_gbuffer = gbuffer_program;
        engine->render_lighting = lighting_program;
        engine->render_selectblock = selectblock_program;
        engine->render_sprite = sprite_program;
        engine->render_text = text_program;
        engine->render_quad = icra_gl_renderquad_new ();

failfast:
        if (status != cra_ok)
        {
                icra_crankshaft_error (status);
                icra_gl_program_delete (gbuffer_program);
                icra_gl_program_delete (lighting_program);
                icra_gl_program_delete (selectblock_program);
                icra_gl_program_delete (sprite_program);
                icra_gl_program_delete (text_program);
        }

        icra_ulc_mainthread_enqueue_closure (engine, &dd->closure, status,
                                             window);
        return G_SOURCE_REMOVE;
}

static void
renderpipeline_init_closure_finalizer (cra_ulc_closure_t closure)
{
        dd_t dd = (dd_t)closure;
        ICRA_ASSERT (&dd->closure == closure);

        icra_constfree (dd->params.gbuffer_vertex_shader);
        icra_constfree (dd->params.gbuffer_fragment_shader);
        icra_constfree (dd->params.lighting_vertex_shader);
        icra_constfree (dd->params.lighting_fragment_shader);
        icra_constfree (dd->params.selectblock_vertex_shader);
        icra_constfree (dd->params.selectblock_fragment_shader);
        icra_constfree (dd->params.sprite_vertex_shader);
        icra_constfree (dd->params.sprite_fragment_shader);
        icra_constfree (dd->params.text_vertex_shader);
        icra_constfree (dd->params.text_fragment_shader);

        icra_ulc_closure_deleter_finalizer (closure);
}

int
cra_renderpipeline_init (cra_window_t window,
                         struct cra_renderpipeline_init_params params,
                         cra_ulc_window callback, void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (window);
        ICRA_PREAMBLE_API ();

        cra_engine_t engine = window->engine;

        GLFWframebuffersizefun old_handler = glfwSetFramebufferSizeCallback (
            window->glfw_window, framebuffer_size_handler);
        ICRA_CHECK_LOGIC (old_handler == NULL);

        int width = 0;
        int height = 0;
        glfwGetFramebufferSize (window->glfw_window, &width, &height);
        framebuffer_size_handler (window->glfw_window, width, height);

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               renderpipeline_init_closure_finalizer);
        dd->window = window;
        dd->engine = engine;

        dd->params.gbuffer_vertex_shader
            = g_strdup (params.gbuffer_vertex_shader);
        dd->params.gbuffer_fragment_shader
            = g_strdup (params.gbuffer_fragment_shader);
        dd->params.lighting_vertex_shader
            = g_strdup (params.lighting_vertex_shader);
        dd->params.lighting_fragment_shader
            = g_strdup (params.lighting_fragment_shader);
        dd->params.selectblock_vertex_shader
            = g_strdup (params.selectblock_vertex_shader);
        dd->params.selectblock_fragment_shader
            = g_strdup (params.selectblock_fragment_shader);
        dd->params.sprite_vertex_shader
            = g_strdup (params.sprite_vertex_shader);
        dd->params.sprite_fragment_shader
            = g_strdup (params.sprite_fragment_shader);
        dd->params.text_vertex_shader = g_strdup (params.text_vertex_shader);
        dd->params.text_fragment_shader
            = g_strdup (params.text_fragment_shader);

        icra_engine_openglthread_enqueue_dispatcher (
            engine, dispatch_renderpipeline_init, dd);

        return cra_ok;
}
