#ifndef PLATFORM_H
#define PLATFORM_H

#include "jsk.h"

#include "platform/platform_data.h"

b32 platform_key_is_down(Platform *platform, Key key);
b32 platform_key_is_pressed(Platform *platform, Key key);
b32 platform_key_is_released(Platform *platform, Key key);

void platform_init(Platform *platform);
void platform_begin_frame(Platform *platform);
void platform_end_frame(Platform *platform);
void platform_close(Platform *platform);

#endif
