#pragma once

#include "core/types.h"
#include "platform/input.h"
#include "platform/window.h"

struct Platform
{
    b32 active;

    f64 time_current;
    f64 time_previous;

    f32 delta_time;

    Window window;
    Input input;
};

void platform_init(Platform& platform);
void platform_begin_frame(Platform& platform);
void platform_end_frame(Platform& platform);
void platform_close();
