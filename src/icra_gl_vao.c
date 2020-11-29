
#include "icra_gl_vao.h"
#include "icra_mem.h"

struct icra_gl_vao_s
{
        GLenum mode;
        GLuint vao;
        GLuint vbo;
        GLsizei count;
};

icra_gl_vao_t
icra_gl_vao_new (GLenum mode)
{
        icra_gl_vao_t vao;
        ICRA_MALLOC (vao);

        vao->mode = mode;

        return vao;
}

void
icra_gl_vao_delete (icra_gl_vao_t vao)
{
        if (vao->vbo != 0)
        {
                glDeleteBuffers (1, &vao->vbo);
                vao->vbo = 0;
        }

        if (vao->vao != 0)
        {
                glDeleteVertexArrays (1, &vao->vao);
                vao->vao = 0;
        }

        icra_free (vao);
}

static inline GLsizei
gl_sizeof_type (GLenum type)
{
        switch (type)
        {
        case GL_FLOAT:
                return sizeof (GLfloat);
        }

        g_return_val_if_reached (0);
}

void
icra_gl_vao_init (icra_gl_vao_t vao, const void *data, GLsizei count,
                  struct icra_gl_vao_init_attrib *attribs, int attrib_count)
{
        vao->count = count;

        GLsizei elem_size = 0;
        for (int i = 0; i < attrib_count; i++)
        {
                struct icra_gl_vao_init_attrib *attrib = attribs + i;
                elem_size += attrib->size * gl_sizeof_type (attrib->type);
        }

        glGenVertexArrays (1, &vao->vao);
        glGenBuffers (1, &vao->vbo);
        glBindBuffer (GL_ARRAY_BUFFER, vao->vbo);
        glBufferData (GL_ARRAY_BUFFER, elem_size * count, data,
                      GL_STATIC_DRAW);

        glBindVertexArray (vao->vao);
        void *pointer = (void *)0;
        for (int i = 0; i < attrib_count; i++)
        {
                struct icra_gl_vao_init_attrib *attrib = attribs + i;
                glVertexAttribPointer (i, attrib->size, attrib->type, GL_FALSE,
                                       elem_size, pointer);
                glEnableVertexAttribArray (i);
                pointer += attrib->size * gl_sizeof_type (attrib->type);
        }

        glBindVertexArray (0);
}

void
icra_gl_vao_render (icra_gl_vao_t const vao)
{
        glBindVertexArray (vao->vao);
        glDrawArrays (vao->mode, 0, vao->count);
        glBindVertexArray (0);
}
