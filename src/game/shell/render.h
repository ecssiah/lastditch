#ifndef RENDER_H
#define RENDER_H

#include <glad/glad.h>

#include "game/shell/shell_data.h"
#include "game/sim/sim_data.h"
#include "platform/platform_data.h"

void render_init(Shell *shell, Platform *platform, Sim *sim);
void render_update(Shell *shell, Sim *sim);

#endif
