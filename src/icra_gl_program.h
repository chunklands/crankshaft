#ifndef __CRANKSHAFT_ICRA_GL_PROGRAM_H__
#define __CRANKSHAFT_ICRA_GL_PROGRAM_H__

#include "glfw.h"

typedef struct icra_gl_program_s *icra_gl_program_t;

icra_gl_program_t icra_gl_program_new (void);
void icra_gl_program_delete (icra_gl_program_t program);

int icra_gl_program_compile_shader (icra_gl_program_t program,
                                    char const *vertex_shader_source,
                                    char const *fragment_shader_source);

#endif // __CRANKSHAFT_ICRA_GL_PROGRAM_H__