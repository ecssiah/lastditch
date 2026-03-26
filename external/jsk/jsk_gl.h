#ifndef JSKGL_H
#define JSKGL_H

#include <glad/glad.h>

void jskgl_check_error(const char* label);

GLuint jskgl_compile_shader(GLenum type, const char* filepath);

#endif
