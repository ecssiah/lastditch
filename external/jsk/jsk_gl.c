#include "jsk_gl.h"

#include <malloc/_malloc.h>
#include <stdio.h>
#include <stdlib.h>

static const char* get_error_string(GLenum err)
{
    switch (err)
    {
        case GL_INVALID_ENUM: return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE: return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION";
        case GL_OUT_OF_MEMORY: return "GL_OUT_OF_MEMORY";
        case GL_INVALID_FRAMEBUFFER_OPERATION: return "GL_INVALID_FRAMEBUFFER_OPERATION";
        default: return "UNKNOWN_ERROR";
    }
}

static char* read_file(const char *path)
{
    FILE* file = fopen(path, "rb");
    
    if (!file)
    {
        printf("Failed to open file: %s\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = (char*)malloc(size + 1);
    
    if (!buffer)
    {
        fclose(file);
        return NULL;
    }

    fread(buffer, 1, size, file);
    buffer[size] = '\0';

    fclose(file);
    
    return buffer;
}

void jskgl_check_error(const char* label)
{
    GLenum err;

    while ((err = glGetError()) != GL_NO_ERROR)
    {
        printf("GL ERROR [%s]: %s (0x%x)\n",
               label,
               get_error_string(err),
               err);
    }
}

GLuint jskgl_compile_shader(GLenum type, const char* filepath)
{
    char* src_string = read_file(filepath);
    
    GLuint shader_id = glCreateShader(type);
    
    glShaderSource(shader_id, 1, (const char* const*)&src_string, NULL);
    glCompileShader(shader_id);

    GLint success;
    glGetShaderiv(shader_id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLchar info[512];
        glGetShaderInfoLog(shader_id, 512, NULL, info);
	
        printf("Shader error:\n%s\n", info);
    }

    free(src_string);

    return shader_id;
}
