#ifndef __CRANKSHAFT_H__
#define __CRANKSHAFT_H__

#include <stdbool.h>

#ifdef __cplusplus
extern "C"
{
#endif

#if defined(BUILDING_CRA_SHARED) && defined(USING_CRA_SHARED)
#error "Define either BUILDING_CRA_SHARED or USING_CRA_SHARED, not both."
#endif

#ifdef _WIN32
#if defined(BUILDING_CRA_SHARED)
#define CRA_EXTERN __declspec(dllexport)
#elif defined(USING_CRA_SHARED)
#define CRA_EXTERN __declspec(dllimport)
#else
#define CRA_EXTERN /* nothing */
#endif
#elif __GNUC__ >= 4
#define CRA_EXTERN __attribute__ ((visibility ("default")))
#else
#define CRA_EXTERN /* nothing */
#endif

        typedef enum
        {
                cra_status_min = -1,
                cra_ok = 0,
                cra_err_param,
                cra_err_logic,
                cra_err_lib,
                cra_err_thread,
                cra_err_gl,
                cra_status_max
        } cra_status;

        typedef enum
        {
                cra_loglevel_trace = 0,
                cra_loglevel_debug = 1,
                cra_loglevel_info = 2,
                cra_loglevel_warn = 3,
                cra_loglevel_error = 4,
                cra_loglevel_fatal = 5
        } cra_loglevel;

        // handles
        typedef struct icra_engine_s *cra_engine_t;
        typedef struct icra_window_s *cra_window_t;
        typedef struct icra_block_s *cra_block_t;

        struct __attribute__ ((packed)) cra_vertex
        {
                float position[3];
                float normal[3];
                float uv[2];
        };

        typedef struct cra_window_new_params_s
        {
                int width;
                int height;
                const char *title;
        } * cra_window_new_params_t;

        struct cra_renderpipeline_init_params
        {
                const char *gbuffer_vertex_shader;
                const char *gbuffer_fragment_shader;
                const char *lighting_vertex_shader;
                const char *lighting_fragment_shader;
                const char *selectblock_vertex_shader;
                const char *selectblock_fragment_shader;
                const char *sprite_vertex_shader;
                const char *sprite_fragment_shader;
                const char *text_vertex_shader;
                const char *text_fragment_shader;
        };

        enum cra_block_facetype
        {
                UNKNOWN,
                LEFT,
                RIGHT,
                BOTTOM,
                TOP,
                FRONT,
                BACK,
        };

        struct __attribute__ ((packed)) cra_vao_element_block
        {
                float position[3];
                float normal[3];
                float uv[2];
        };

        struct cra_block_face
        {
                enum cra_block_facetype facetype;
                struct cra_vao_element_block *elements;
                int elements_length;
        };

        struct cra_block_new_params
        {
                const char *id;
                bool opaque;
                const unsigned char *texture_content;
                int texture_content_length;
                struct cra_block_face *faces;
                int faces_length;
        };

        // callback params
        // struct cra_engine_window_on_click_cbparams {
        //     cra_window_t window;
        //     int button;
        //     int action;
        //     int mods;
        // };

        // struct cra_engine_window_on_contentresize_cbparams {
        //     cra_window_t window;
        //     int width;
        //     int height;
        //     float xscale;
        //     float yscale;
        // };

        // struct cra_engine_window_on_key_cbparams {
        //     cra_window_t window;
        //     int key;
        //     int scancode;
        //     int action;
        //     int mods;
        // };

        // struct cra_engine_window_on_resize_cbparams {
        //     cra_window_t window;
        //     int width;
        //     int height;
        // };

        // struct cra_engine_window_on_scroll_cbparams {
        //     cra_window_t window;
        //     int xoffset;
        //     int yoffset;
        // };

        typedef struct icra_ulc_closure_s *cra_ulc_closure_t;
        typedef struct icra_ulh_closure_s *cra_ulh_closure_t;

        typedef void (*cra_error_handler) (int status, void *data);
        typedef void (*cra_callback) (void *data);

        typedef enum
        {
                cra_finalize_data,
                cra_keep_data
        } cra_ulc_invocation_result;

        typedef cra_ulc_invocation_result (*cra_ulc_invoker) (
            cra_ulc_closure_t closure, void *data);

        // callback function types

        typedef void (*cra_ulc_void) (void *void_result, cra_status status,
                                      void *data);
        typedef void (*cra_ulc_engine) (cra_engine_t engine, cra_status status,
                                        void *data);
        typedef void (*cra_ulc_window) (cra_window_t window, cra_status status,
                                        void *data);

        // handler function types

        typedef void (*cra_ulh_window_close) (cra_window_t window, void *data);
        // typedef int (*cra_engine_window_on_click_cb)(const struct
        // cra_engine_window_on_click_cbparams* params, void* data); typedef
        // int (*cra_engine_window_on_contentresize_cb)(const struct
        // cra_engine_window_on_contentresize_cbparams* params, void* data);
        // typedef int (*cra_engine_window_on_key_cb)(const struct
        // cra_engine_window_on_key_cbparams* params, void* data); typedef int
        // (*cra_engine_window_on_resize_cb)(const struct
        // cra_engine_window_on_resize_cbparams* params, void* data); typedef
        // int (*cra_engine_window_on_scroll_cb)(const struct
        // cra_engine_window_on_scroll_cbparams* params, void* data); typedef
        // int (*cra_engine_block_new_cb)(cra_block_t block, void* data);

        // api
        CRA_EXTERN int cra_log_set_level (cra_loglevel level);
        CRA_EXTERN int
        cra_closure_execute_and_finalize (cra_ulc_closure_t closure);
        CRA_EXTERN int cra_init (cra_error_handler error_handler, void *data);
        CRA_EXTERN int cra_destroy (void);

        CRA_EXTERN int cra_engine_bake_texture (cra_engine_t engine,
                                                cra_ulc_engine callback,
                                                void *callback_data);
        CRA_EXTERN int
        cra_engine_block_new (cra_engine_t engine,
                              const struct cra_block_new_params *params,
                              cra_ulc_engine callback, void *callback_data);
        CRA_EXTERN int cra_engine_delete (cra_engine_t engine);
        CRA_EXTERN int cra_engine_get_user_pointer (cra_engine_t engine,
                                                    void **data_ptr);
        CRA_EXTERN int cra_engine_init (cra_engine_t engine,
                                        cra_ulc_engine callback,
                                        void *callback_data);
        CRA_EXTERN int cra_engine_new (cra_engine_t *engine,
                                       cra_ulc_invoker callback_handler,
                                       void *callback_handler_data);
        CRA_EXTERN int cra_engine_set_user_pointer (cra_engine_t engine,
                                                    void *data);
        CRA_EXTERN int cra_engine_stop (cra_engine_t engine,
                                        cra_ulc_engine callback,
                                        void *callback_data);

        // CRA_EXTERN int cra_engine_block_new(cra_engine_t engine, const
        // struct cra_engine_block_new_params* params, void* data,
        // cra_engine_block_new_cb callback);

        // CRA_EXTERN int cra_engine_render_pipeline_init(cra_window_t window,
        // const struct craw_engine_render_pipeline_init_params* params, cra_cb
        // callback);

        CRA_EXTERN int cra_window_new (cra_engine_t engine,
                                       const cra_window_new_params_t params,
                                       cra_ulc_window callback,
                                       void *callback_data);
        CRA_EXTERN int cra_window_delete (cra_window_t window,
                                          cra_ulc_void callback,
                                          void *callback_data);
        CRA_EXTERN int cra_window_shouldclose (cra_window_t window,
                                               int *result);
        CRA_EXTERN int cra_window_get_engine (cra_window_t window,
                                              cra_engine_t *engine);
        CRA_EXTERN int cra_window_load_gl (cra_window_t window,
                                           cra_ulc_window callback,
                                           void *callback_data);
        CRA_EXTERN int cra_window_on_close (cra_window_t window,
                                            cra_ulh_window_close handler,
                                            void *handler_data);
        CRA_EXTERN int
        cra_renderpipeline_init (cra_window_t window,
                                 struct cra_renderpipeline_init_params params,
                                 cra_ulc_window callback, void *callback_data);
        // CRA_EXTERN int cra_engine_window_on_click(cra_window_t window, void*
        // data, cra_engine_window_on_click_cb callback); CRA_EXTERN int
        // cra_engine_window_on_contentresize(cra_window_t window, void* data,
        // cra_engine_window_on_contentresize_cb callback); CRA_EXTERN int
        // cra_engine_window_on_key(cra_window_t window, void* data,
        // cra_engine_window_on_key_cb callback); CRA_EXTERN int
        // cra_engine_window_on_resize(cra_window_t window, void* data,
        // cra_engine_window_on_resize_cb callback); CRA_EXTERN int
        // cra_engine_window_on_scroll(cra_window_t window, void* data,
        // cra_engine_window_on_scroll_cb callback); CRA_EXTERN int

#ifdef __cplusplus
}
#endif

#endif // __CRANKSHAFT_H__