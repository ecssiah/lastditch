#pragma once

#include <functional>
#include "actor.h"
#include "constants.h"
#include "control.h"
#include "navigation.h"
#include "core/random.h"
#include "core/types.h"

class Population;
class World;

using WorkFunction = std::function<void(World&, Population&)>;

struct Work
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    WorkFunction work_function {};
};

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
