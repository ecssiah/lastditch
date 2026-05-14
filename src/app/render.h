#pragma once

#include "app/shell.h"
#include "app/sim.h"
#include "platform/platform.h"

constexpr f32 clear_color[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

GLuint render_compile_shader(GLenum type, const char* filepath);

void render_init(Shell& shell, const Platform& platform, const Sim& sim);
void render_update(Shell& shell, const Sim& sim);
