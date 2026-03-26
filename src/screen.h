#ifndef SCREEN_H
#define SCREEN_H

#include <cglm/cglm.h>

#include "ld_data.h"

void screen_draw_text(Interface *interface, const char *text, f32 x, f32 y);

void screen_init(Interface *interface);
void screen_update(Interface *interface, Sim *sim);

#endif
