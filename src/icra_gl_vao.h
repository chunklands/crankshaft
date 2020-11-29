#ifndef __CRANKSHAFT_ICRA_GL_VAO_H__
#define __CRANKSHAFT_ICRA_GL_VAO_H__

#include "glfw.h"

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

#endif // __CRANKSHAFT_ICRA_GL_VAO_H__