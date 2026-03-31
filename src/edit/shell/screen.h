#ifndef SCREEN_H
#define SCREEN_H

#include <cglm/cglm.h>

#include "edit/sim/sim_data.h"
#include "edit/shell/shell_data.h"
#include "platform/platform_data.h"

void screen_draw_text(Shell *shell, const char *text, f32 x, f32 y);

void screen_init(Shell *shell, Platform *platform);
void screen_update(Shell *shell, Sim *sim);

#endif
