#pragma once

#include "app/action.h"
#include "app/debug.h"
#include "app/navigation.h"
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
    ActionQueue action_queue;
    
    World world;
    Population population;
    
    Work work;
    Navigation navigation;
    
    Screen screen;
    Render render;
};
