#pragma once

#include "app/state.h"
#include "platform/platform.h"

class App
{
public:
    App();

    void update();
    void quit();

    b32 active { true };

private:
    Platform platform {};
    State state {};
};
