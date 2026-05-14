#pragma once

#include "app/sim.h"
#include "app/shell.h"
#include "platform/platform.h"

void screen_init(Shell& shell, const Platform& platform);
void screen_update(Shell& shell, Sim& sim);
