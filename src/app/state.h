#pragma once

#include "core/types.h"
#include "app/debug.h"
#include "app/action.h"
#include "app/work.h"
#include "app/navigation.h"
#include "app/population.h"
#include "app/world.h"
#include "app/screen.h"
#include "app/render.h"

struct State
{
    b32 active;
    b32 evolving;

    u32 seed;

    Debug debug;

    ActionQueue action_queue;
    
    Work work;
    Navigation navigation;
    World world;
    Population population;
    
    Screen screen;
    Render render;
};
