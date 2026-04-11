#ifndef JUSTSKY_GL_H
#define JUSTSKY_GL_H

#include <glad/glad.h>

void justsky_gl_check_error(const char* label);

GLuint justsky_gl_compile_shader(GLenum type, const char* filepath);

#endif
