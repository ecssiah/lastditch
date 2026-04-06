#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "game/shell/shell.h"
#include "game/sim/sim.h"
#include "platform/platform.h"

void render_init(Shell *shell, Platform *platform, Sim *sim);
void render_update(Shell *shell, Sim *sim);

#endif
