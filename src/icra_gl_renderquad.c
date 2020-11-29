
#include "icra_gl_renderquad.h"
#include "icra_gl_vao.h"
#include "icra_mem.h"

static GLfloat const quad_vertices[] = {
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f,  -1.0f, 0.0f, 1.0f, 0.0f, 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
};

icra_gl_vao_t
icra_gl_renderquad_new (void)
{
        icra_gl_vao_t quad = icra_gl_vao_new (GL_TRIANGLE_STRIP);
        struct icra_gl_vao_init_attrib attribs[]
            = { { .type = GL_FLOAT, .size = 3 },
                { .type = GL_FLOAT, .size = 2 } };
        icra_gl_vao_init (quad, quad_vertices, 4, attribs, 2);

        return quad;
}
