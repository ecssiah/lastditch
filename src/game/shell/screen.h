#pragma once

#include "game/sim/sim.h"
#include "game/shell/shell.h"
#include "platform/platform.h"

void screen_init(Shell& shell, const Platform& platform);
void screen_update(Shell& shell, Sim& sim);
