#pragma once

#include <functional>

#include "actor.h"
#include "constants.h"
#include "control.h"
#include "core/random.h"
#include "core/types.h"

class Population;
class World;

struct TaskRecord
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    std::function<void(World&, Population&)> task {};
};

class Work
{
public:
    void update(World& world, Population& population);

    b32 find_task(s32 actor_id);

    void schedule(s32 frequency, s32 phase, std::function<void(World&, Population&)> task);

    Random random { WORK_SEED };

private:
    u64 tick_count { 0 };

    std::vector<TaskRecord> task_record_vector {};
};
