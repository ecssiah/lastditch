#pragma once

#include "glad/gl.h"

#include "app/population.h"
#include "platform/platform.h"

struct Screen
{
    GLuint program_id;

    GLuint vao_id;
    GLuint vbo_id;

    GLuint font_texture_id;

    GLint u_font_texture_sampler_location;

    GLint u_projection_location;
};

void screen_init(Screen& screen, const Platform& platform);
void screen_update(Screen& screen, const Population& population);
