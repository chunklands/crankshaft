#include "icra.h"

typedef enum
{
        split_still_unknown = 0,
        split_horizontal,
        split_vertical,
} split;

struct bake_node
{
        cra_block_t entity;
        struct bake_node *down;
        struct bake_node *right;
        split split;
        int x;
        int y;
};

struct bake_tree_result
{
        struct bake_node *root;
        int dim;
};

static int
block_size_desc (cra_block_t *a_ptr, cra_block_t *b_ptr)
{
        cra_block_t a = *a_ptr;
        cra_block_t b = *b_ptr;

        int size_a = a->texture_width * a->texture_height;
        int size_b = b->texture_width * b->texture_height;

        return size_b - size_a;
}

const int COLOR_COMPONENTS = 4;

static int
next_pot (int n)
{
        int r = 1;
        while (r < n)
                r <<= 1;

        return r;
}

static void
delete_block_textures (cra_engine_t engine)
{
        for (int i = 0; i < engine->game_blocks->len; i++)
        {
                cra_block_t block
                    = g_array_index (engine->game_blocks, cra_block_t, i);

                stbi_image_free (block->texture);
                block->texture = NULL;
                block->texture_width = -1;
                block->texture_height = -1;
        }
}

static void
delete_bake_nodes (struct bake_node *node)
{
        if (node == NULL)
                return;

        delete_bake_nodes (node->right);
        delete_bake_nodes (node->down);

        icra_free (node);
}

// TODO (daaitch): algorithm will not produce overlapping textures, but it will
// also not consider adding new textures to the right node if it fits into the
// down node, which produces large textures at current.
static struct bake_tree_result
bake (cra_engine_t engine)
{
        struct bake_node *root;
        ICRA_MALLOC (root);

        int max_dim = 0;
        for (int i = 0; i < engine->game_blocks->len; i++)
        {
                cra_block_t block
                    = g_array_index (engine->game_blocks, cra_block_t, i);
                if (block->texture == NULL)
                {
                        continue;
                }

                struct bake_node *node = root;
                for (;;)
                {
                        ICRA_ASSERT (node != NULL);
                        if (node->split == split_still_unknown)
                        {
                                node->entity = block;

                                int overall_width
                                    = node->x + node->entity->texture_width;
                                max_dim = MAX (max_dim, overall_width);
                                int overall_height
                                    = node->y + node->entity->texture_height;
                                max_dim = MAX (max_dim, overall_height);

                                node->split = overall_width > overall_height
                                                  ? split_horizontal
                                                  : split_vertical;
                                break;
                        }

                        // +++++++++
                        // +XXX
                        // +XXX.....
                        // +
                        // +
                        if (node->split == split_horizontal)
                        {
                                if (node->entity->texture_height
                                    >= block->texture_height)
                                {
                                        if (node->right == NULL)
                                        {
                                                ICRA_MALLOC (node->right);
                                                node->right->x
                                                    = node->x
                                                      + node->entity
                                                            ->texture_width;
                                                node->right->y = node->y;
                                        }

                                        node = node->right;
                                        continue;
                                }

                                if (node->down == NULL)
                                {
                                        ICRA_MALLOC (node->down);
                                        node->down->x = node->x;
                                        node->down->y
                                            = node->y
                                              + node->entity->texture_height;
                                }

                                node = node->down;
                                continue;
                        }

                        // ++++++
                        // +XXX
                        // +XXX
                        // +  .
                        // +  .
                        if (node->split == split_vertical)
                        {
                                if (node->entity->texture_width
                                    >= block->texture_width)
                                {
                                        if (node->down == NULL)
                                        {
                                                ICRA_MALLOC (node->down);
                                                node->down->x = node->x;
                                                node->down->y
                                                    = node->y
                                                      + node->entity
                                                            ->texture_height;
                                        }

                                        node = node->down;
                                        continue;
                                }

                                if (node->right == NULL)
                                {
                                        ICRA_MALLOC (node->right);
                                        node->right->x
                                            = node->x
                                              + node->entity->texture_width;
                                        node->right->y = node->y;
                                }

                                node = node->right;
                                continue;
                        }

                        // unreachable
                        ICRA_ASSERT (false);
                }
        }

        int dim = next_pot (max_dim);

        struct bake_tree_result result = { .dim = dim, .root = root };
        return result;
}

void
generate_texture (unsigned char *texture, int dim, struct bake_node *node)
{
        size_t offset = (node->x + (node->y * dim)) * COLOR_COMPONENTS;
        unsigned char *dst = texture + offset;
        const stbi_uc *src = node->entity->texture;

        size_t dst_line_offset = dim * COLOR_COMPONENTS;
        size_t src_line_offset
            = node->entity->texture_width * COLOR_COMPONENTS;

        for (int i = 0; i < node->entity->texture_height; i++)
        {
                memcpy (dst, src, src_line_offset);
                dst += dst_line_offset;
                src += src_line_offset;
        }

        for (int f = 0; f < node->entity->faces_length; f++)
        {
                struct cra_block_face *face = node->entity->faces + f;
                for (int i = 0; i < face->elements_length; i++)
                {
                        struct cra_vao_element_block *element_block
                            = face->elements + i;
                        element_block->uv[0]
                            = (node->x
                               + (element_block->uv[0]
                                  * node->entity->texture_width))
                              / dim;
                        element_block->uv[1]
                            = (node->x
                               + (element_block->uv[1]
                                  * node->entity->texture_height))
                              / dim;
                }
        }

        if (node->right)
                generate_texture (texture, dim, node->right);

        if (node->down)
                generate_texture (texture, dim, node->down);
}

typedef struct dd_s
{
        ICRA_DD_CALLBACK_CLOSURE_FIELD
        cra_engine_t engine;
} * dd_t;

gboolean
dispatch_engine_bake_texture (dd_t dd)
{
        ICRA_ASSERT (dd != NULL);
        ICRA_PREAMBLE_DISPATCH ();

        cra_engine_t engine = dd->engine;

        g_array_sort (engine->game_blocks, (GCompareFunc)block_size_desc);

        struct bake_tree_result result = bake (engine);

        size_t size = result.dim * result.dim * COLOR_COMPONENTS;
        unsigned char *texture;
        ICRA_MALLOC_N (texture, size);

        generate_texture (texture, result.dim, result.root);
        int write_result = stbi_write_png ("out.png", result.dim, result.dim,
                                           4, texture, 0);
        ICRA_ASSERT (write_result == 1);

        glGenTextures (1, &engine->game_texture);
        glBindTexture (GL_TEXTURE_2D, engine->game_texture);
        glTexImage2D (GL_TEXTURE_2D, 0, GL_RGBA, result.dim, result.dim, 0,
                      GL_RGBA, GL_UNSIGNED_BYTE, texture);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
                         GL_NEAREST_MIPMAP_LINEAR);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glGenerateMipmap (GL_TEXTURE_2D);
        glBindTexture (GL_TEXTURE_2D, 0);

        delete_block_textures (engine);
        delete_bake_nodes (result.root);

        icra_ulc_mainthread_enqueue_closure (engine, &dd->closure, cra_ok,
                                             dd->engine);

        return G_SOURCE_REMOVE;
}

int
cra_engine_bake_texture (cra_engine_t engine, cra_ulc_engine callback,
                         void *callback_data)
{
        ICRA_CHECK_PARAM_NOTNULL (engine);
        ICRA_PREAMBLE_API ();

        dd_t dd;
        ICRA_MALLOC (dd);
        icra_ulc_closure_init (&dd->closure, callback, callback_data,
                               icra_ulc_closure_deleter_finalizer);
        dd->engine = engine;

        icra_engine_openglthread_enqueue_dispatcher (
            engine, dispatch_engine_bake_texture, dd);
        return cra_ok;
}