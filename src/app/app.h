#pragma once

#include "population.h"
#include "render.h"
#include "work.h"
#include "world.h"
#include "platform/platform.h"

class App
{
public:
    App() = default;

    void init();
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
