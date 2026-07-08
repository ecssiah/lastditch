#pragma once

#include "app/action.h"
#include "app/debug.h"
#include "app/population.h"
#include "app/world.h"
#include "app/screen.h"
#include "app/render.h"
#include "core/types.h"

class State
{
public:
    State();

    b32 evolving;

    Debug debug;
    Action action;
    
    World world;
    Population population;
    
    Work work;

    Screen screen;
    Render render;
};
