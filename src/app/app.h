#pragma once

#include "core/types.h"
#include "app/debug.h"
#include "app/action.h"
#include "app/population.h"
#include "app/world.h"
#include "app/screen.h"
#include "app/render.h"
#include "platform/platform.h"

struct State
{
    u64 seed;
    
    b32 is_active;
    b32 is_evolving;

    Debug debug;
    ActionQueue action_queue;
    
    World world;
    Population population;
    
    Screen screen;
    Render render;
};

void app_init(State& state, Platform& platform);
void app_update(State& state, Platform& platform);
void app_quit(State& state);