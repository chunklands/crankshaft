#include "icra.h"

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
        cra_block_t block;
} * dd_t;

static gboolean
dispatch_block_new (dd_t dd)
{
        ICRA_PREAMBLE_DISPATCH ();
        g_array_append_val (dd->engine->game_blocks, dd->block);
        icra_ulc_mainthread_enqueue_closure (dd->engine, &dd->closure, cra_ok,
                                             dd->engine);
        return G_SOURCE_REMOVE;
}

int
cra_engine_block_new (cra_engine_t engine,
                      const struct cra_block_new_params *params,
                      cra_ulc_engine callback, void *callback_data)
{
        ICRA_PREAMBLE_API ();
        cra_block_t block;
        ICRA_MALLOC (block);

        block->id = g_strdup (params->id);
        block->opaque = params->opaque;

        ICRA_MEMDUP (block->faces, params->faces, params->faces_length);
        block->faces_length = params->faces_length;

        for (int i = 0; i < block->faces_length; i++)
        {
                ICRA_MEMDUP (block->faces[i].elements,
                             params->faces[i].elements,
                             params->faces[i].elements_length);
                ICRA_ASSERT (block->faces[i].elements_length
                             == params->faces[i].elements_length);
        }

        if (params->texture_content != NULL)
        {
                block->texture = stbi_load_from_memory (
                    params->texture_content, params->texture_content_length,
                    &block->texture_width, &block->texture_height, NULL, 4);
                ICRA_CHECK_LIB (block->texture != NULL);
        }

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->engine = engine;
        dd->block = block;

        icra_engine_openglthread_enqueue_dispatcher (engine,
                                                     dispatch_block_new, dd);

        return cra_ok;
}
