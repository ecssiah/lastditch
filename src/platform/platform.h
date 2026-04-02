#ifndef PLATFORM_H
#define PLATFORM_H

#include "jsk.h"

#include "platform/platform_data.h"

bool platform_button_is_down(Platform *platform, Button key);
bool platform_button_is_pressed(Platform *platform, Button key);
bool platform_button_is_released(Platform *platform, Button key);

void platform_init(Platform *platform, const char *window_title);
void platform_begin_frame(Platform *platform);
void platform_end_frame(Platform *platform);
void platform_close();

#endif
