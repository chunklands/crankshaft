#ifndef __CRANKSHAFT_H__
#define __CRANKSHAFT_H__

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
                _CRA_STATUS_MIN = -1,
                CRA_OK = 0,
                CRA_ERR_PARAM,
                CRA_ERR_LOGIC,
                CRA_ERR_LIB,
                CRA_ERR_THREAD,
                CRA_ERR_USER,
                _CRA_STATUS_MAX
        } cra_status;

        // handles
        typedef struct icra_engine_s *cra_engine_t;
        typedef struct icra_window_s *cra_window_t;
        // typedef struct icra_block_s* cra_block_t;

        // other types
        enum cra_block_face_type
        {
                UNKNOWN,
                LEFT,
                RIGHT,
                BOTTOM,
                TOP,
                FRONT,
                BACK,
        };

        struct __attribute__ ((packed)) cra_vertex
        {
                float position[3];
                float normal[3];
                float uv[2];
        };

        // params
        struct cra_engine_block_new_params_face
        {
                const char *name;
                enum cra_block_face_type type;
                const struct cra_vertex *data;
        };

        struct cra_engine_block_new_params
        {
                const char *id;
                const struct cra_engine_block_new_params_face *faces;
                int opaque;
                const char *texture;
        };

        typedef struct cra_window_new_params_s
        {
                int width;
                int height;
                const char *title;
        } * cra_window_new_params_t;

        struct cra_engine_render_pipeline_init_params
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

        typedef enum
        {
                CRA_FREE_DATA,
                CRA_KEEP_DATA
        } CraCallbackHandlerResult;

        typedef void (*cra_error_handler) (int status, void *data);
        typedef void (*cra_callback) (void *result, cra_status status,
                                      void *user_data);
        typedef struct cra_callback_closure_s
        {
                cra_callback callback;
                void *callback_result;
                cra_status callback_status;
                void *callback_user_data;
                void (*finalizer) (void *closure);
        } * cra_callback_closure_t;
        typedef CraCallbackHandlerResult (*cra_callback_handler) (
            cra_callback_closure_t closure, void *data);
        typedef void (*cra_engine_cb) (cra_engine_t engine, cra_status status,
                                       void *data);
        typedef void (*cra_window_cb) (cra_window_t window, cra_status status,
                                       void *data);
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
        CRA_EXTERN int cra_init (cra_error_handler error_handler, void *data);
        CRA_EXTERN int cra_destroy ();
        CRA_EXTERN int cra_poll_events (cra_engine_t engine);

        CRA_EXTERN int cra_engine_new (cra_engine_t *engine,
                                       cra_callback_handler callback_handler,
                                       void *callback_handler_data);
        CRA_EXTERN int cra_engine_delete (cra_engine_t engine);

        CRA_EXTERN int cra_engine_init (cra_engine_t engine,
                                        cra_engine_cb callback,
                                        void *callback_user_data);
        CRA_EXTERN int cra_engine_stop (cra_engine_t engine,
                                        cra_engine_cb callback,
                                        void *callback_user_data);

        // CRA_EXTERN int cra_engine_block_new(cra_engine_t engine, const
        // struct cra_engine_block_new_params* params, void* data,
        // cra_engine_block_new_cb callback);

        // CRA_EXTERN int cra_engine_render_pipeline_init(cra_window_t window,
        // const struct craw_engine_render_pipeline_init_params* params, cra_cb
        // callback);

        CRA_EXTERN int cra_window_new (cra_engine_t engine,
                                       const cra_window_new_params_t params,
                                       cra_window_cb callback,
                                       void *callback_user_data);
        CRA_EXTERN int cra_window_delete (cra_window_t window,
                                          cra_window_cb callback,
                                          void *callback_user_data);
        CRA_EXTERN int cra_window_shouldclose (cra_window_t window,
                                               int *result);
        CRA_EXTERN int cra_window_get_engine (cra_window_t window,
                                              cra_engine_t *engine);
        CRA_EXTERN int cra_window_load_gl (cra_window_t window,
                                           cra_window_cb callback,
                                           void *callback_user_data);
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
        // cra_engine_window_on_shouldclose(cra_window_t window, void* data,
        // cra_cb callback);

#ifdef __cplusplus
}
#endif

#endif // __CRANKSHAFT_H__