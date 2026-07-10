#pragma once

#include "app/control.h"
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
    Control control;
    
    World world;
    Population population;

    Render render;
    Screen screen;
};
