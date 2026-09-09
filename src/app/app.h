#pragma once

#include "app/action.h"
#include "app/control.h"
#include "app/population.h"
#include "app/render.h"
#include "app/world.h"
#include "core/types.h"
#include "platform/platform.h"

class App
{
public:
    App() = default;

    void init();
    void update();
    void quit();

    b32 is_active() const;

    b32 is_evolving { true };

private:
    f64 simulation_time { 0.0 };

    Platform platform {};

    Action action {};

    World world {};
    Population population {};

    Control control {};
    Render render {};
};
