#ifndef GL_EXT_H
#define GL_EXT_H

#include <glad/glad.h>

void gl_check_error(const char* label);
GLuint gl_compile_shader(GLenum type, const char* filepath);

#endif
