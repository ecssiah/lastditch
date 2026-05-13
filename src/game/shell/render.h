#pragma once

#include "game/shell/shell.h"
#include "game/sim/sim.h"
#include "platform/platform.h"

constexpr f32 CLEAR_COLOR[4] = { 0.1f, 0.2f, 0.3f, 1.0f };

GLuint render_compile_shader(GLenum type, const char* filepath);

void render_init(Shell& shell, Platform& platform, Sim& sim);
void render_update(Shell& shell, Sim& sim);
