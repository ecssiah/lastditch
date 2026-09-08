#pragma once

#include "data/actor.h"
#include "data/constants.h"
#include "control.h"
#include "navigation.h"
#include "core/random.h"
#include "core/types.h"

struct Work;

extern const std::vector<Work> WORK_VECTOR;

class Action
{
public:
    void init(World& world);
    void update(World& world, Population& population);

    b32 is_due(const Work& job) const;

    Random random { WORK_SEED };
    Navigation navigation {};

private:
    u64 tick_count { 0 };
};
