#pragma once

#include "app/debug.h"
#include "app/population.h"
#include "app/world.h"
#include "app/render.h"
#include "core/types.h"

class State
{
public:
    State();

    b32 evolving;

    World world;
    Population population;

    Render render;
};
