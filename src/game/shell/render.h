#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "game/shell/shell.h"
#include "game/sim/sim.h"
#include "platform/platform.h"

#define CLEAR_COLOR { 0.1f, 0.2f, 0.3f, 1.0f }

void render_init(Shell *shell, Platform *platform, Sim *sim);
void render_update(Shell *shell, Sim *sim);

#endif
