#ifndef SCREEN_H
#define SCREEN_H

#include "game/sim/sim.h"
#include "game/shell/shell.h"
#include "platform/platform.h"

void screen_init(Shell *shell, Platform *platform);
void screen_update(Shell *shell, Sim *sim);

#endif
