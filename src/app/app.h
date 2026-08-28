#pragma once

#include "population.h"
#include "render.h"
#include "action.h"
#include "world.h"
#include "platform/platform.h"

class App
{
public:
    App() = default;

    void init();
    void update();
    void quit();

    b32 is_active() const;

    b32 evolving { true };

private:
    f64 simulation_time { 0.0 };

    Platform platform {};

    Action action {};

    World world {};
    Population population {};

    Control control {};
    Render render {};
};
