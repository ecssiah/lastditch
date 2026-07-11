#pragma once

#include "app/population.h"
#include "app/world.h"
#include "app/render.h"
#include "core/types.h"

class State
{
public:
    State() = default;

    b32 evolving { true };

    World world {};
    Population population {};

    Render render {};
};
