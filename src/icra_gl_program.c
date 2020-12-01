#include "icra.h"

struct icra_gl_program_s
{
        GLuint program;
};

icra_gl_program_t
icra_gl_program_new (void)
{
        icra_gl_program_t program;
        ICRA_MALLOC (program);

        return program;
}

void
icra_gl_program_delete (icra_gl_program_t program)
{
        if (program->program != 0)
        {
                glDeleteProgram (program->program);
                program->program = 0;
        }

        icra_free (program);
}

static int
gl_compile_shader (GLuint shader, char const *source)
{
        glShaderSource (shader, 1, &source, NULL);
        glCompileShader (shader);

        GLint result = GL_FALSE;
        glGetShaderiv (shader, GL_COMPILE_STATUS, &result);

        if (result != GL_TRUE)
        {
                GLint length = 0;
                glGetShaderiv (shader, GL_INFO_LOG_LENGTH, &length);
                char *message = icra_malloc_n (length + 1);
                glGetShaderInfoLog (shader, length, NULL, message);
                icra_log_error ("OpenGL compile shader: %s", message);
                icra_free (message);
                return cra_err_gl;
        }

        return cra_ok;
}

int
icra_gl_program_compile_shader (icra_gl_program_t gl_program,
                                char const *vertex_shader_source,
                                char const *fragment_shader_source)
{
        int status = cra_ok;

        GLuint vertex_shader = glCreateShader (GL_VERTEX_SHADER);
        status = gl_compile_shader (vertex_shader, vertex_shader_source);
        if (status != cra_ok)
        {
                glDeleteShader (vertex_shader);
                return status;
        }

        GLuint fragment_shader = glCreateShader (GL_FRAGMENT_SHADER);
        status = gl_compile_shader (fragment_shader, fragment_shader_source);
        if (status != cra_ok)
        {
                glDeleteShader (vertex_shader);
                glDeleteShader (fragment_shader);
                return status;
        }

        GLuint program = glCreateProgram ();
        glAttachShader (program, vertex_shader);
        glAttachShader (program, fragment_shader);
        glLinkProgram (program);

        glDeleteShader (vertex_shader);
        glDeleteShader (fragment_shader);

        GLint result = GL_FALSE;
        glGetProgramiv (program, GL_LINK_STATUS, &result);

        if (result != GL_TRUE)
        {
                GLint length = 0;
                glGetProgramiv (program, GL_INFO_LOG_LENGTH, &length);
                char *message = icra_malloc_n (length + 1);
                glGetProgramInfoLog (program, length, NULL, message);
                icra_log_error ("OpenGL link program: %s", message);
                icra_free (message);

                glDeleteProgram (program);
                return cra_err_gl;
        }

        gl_program->program = program;
        return cra_ok;
}
