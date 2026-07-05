#pragma once

#include "core/types.h"
#include "platform/input.h"
#include "platform/window.h"

class Platform
{
    b32 active;

    f64 time_current;
    f64 time_previous;

    f32 delta_time;

    void init_glfw();

    void update_time();

public:

    Window window;
    Input input;

    void init();
    void begin_frame();
    void end_frame();
    void quit();

    b32 is_active() const { return active; }

    f32 get_delta_time() const { return delta_time; }
};


