#ifndef SCREEN_H
#define SCREEN_H

#include <cglm/cglm.h>

#include "game/sim/sim_data.h"
#include "game/shell/shell_data.h"
#include "platform/platform_data.h"

void screen_init(Shell *shell, Platform *platform);
void screen_update(Shell *shell, Sim *sim);

#endif
