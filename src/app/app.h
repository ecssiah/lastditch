#pragma once

#include "population.h"
#include "render.h"
#include "world.h"
#include "platform/platform.h"

class App
{
public:
    App();

    void update();
    void quit();

    b32 active { true };
    b32 evolving { true };

private:
    Platform platform {};

    World world {};
    Population population {};

    Render render {};
};
