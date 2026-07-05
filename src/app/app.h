#pragma once

#include "app/state.h"
#include "platform/platform.h"

constexpr b32 USE_RANDOM_SEED = false;

class App
{
    b32 active;

    Platform platform;
    State state;

public:
    App();

    void update();
    void quit();

    b32 is_active() const { return active; }
};
