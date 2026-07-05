#pragma once

#include "app/state.h"
#include "platform/platform.h"

class App
{
public:
    void init(State& state, const Platform& platform);
    void update(State& state, const Platform& platform);
    void quit(State& state);
};
