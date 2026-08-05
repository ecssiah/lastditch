#pragma once

#include "population.h"
#include "render.h"
#include "work.h"
#include "world.h"
#include "platform/platform.h"

constexpr f64 FRAMES_PER_SECOND { 30.0 };

constexpr f64 FIXED_DELTA_TIME_64 { 1.0  / FRAMES_PER_SECOND };
constexpr f32 FIXED_DELTA_TIME_32 { FIXED_DELTA_TIME_64 };

constexpr f64 MIN_FRAME_TIME { 0.25 };

class App
{
public:
    App();

    void update();
    void quit();

    b32 active { true };
    b32 evolving { true };

private:
    f64 simulation_time { 0.0 };

    Platform platform {};

    Work work {};

    World world {};
    Population population {};

    Control control {};
    Render render {};
};
