#pragma once

#include <functional>

#include "actor.h"
#include "control.h"
#include "core/types.h"

class Population;

class Work
{
public:
    void update(World& world, Population& population);

    void schedule(std::function<void()> task);

private:
    u64 tick_count { 0 };
};
