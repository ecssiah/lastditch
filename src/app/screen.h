#pragma once

#include "glad/gl.h"

#include "app/population.h"
#include "platform/platform.h"

class Screen
{
    GLuint program_id;

    GLuint vao_id;
    GLuint vbo_id;

    GLuint font_texture_id;

    GLint u_font_texture_sampler_location;

    GLint u_projection_location;

    void load_textures(const string& textures_path);
    void draw_text(const string& text, f32 x, f32 y);
    void draw_debug_info(const Population& population);

public:
    void init(const Platform& platform);
    void update(const Population& population);
};


