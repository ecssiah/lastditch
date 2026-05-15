#pragma once

#include "core/types.h"
#include "app/action.h"
#include "app/debug.h"
#include "app/navigation.h"
#include "app/population.h"
#include "app/world.h"
#include "app/screen.h"
#include "app/render.h"

struct State
{
    u64 seed;
    
    b32 is_active;
    b32 is_evolving;

    Debug debug;
    ActionQueue action_queue;
    
    World world;
    Population population;
    
    Work work;
    Navigation navigation;
    
    Screen screen;
    Render render;
};
