#ifndef SCREEN_H
#define SCREEN_H

#include <cglm/cglm.h>

#include "core/core_data.h"

void screen_draw_text(Shell *shell, const char *text, f32 x, f32 y);

void screen_init(Shell *shell);
void screen_update(Shell *shell, Game *game);

#endif
