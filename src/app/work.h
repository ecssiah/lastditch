#pragma once

#include <functional>

#include "actor.h"
#include "control.h"
#include "core/types.h"

class Population;

struct TaskRecord
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    std::function<void()> task {};
};

class Work
{
public:
    void update(World& world, Population& population);

    void schedule(s32 frequency, s32 phase, std::function<void()> task);

private:
    u64 tick_count { 0 };

    std::vector<TaskRecord> task_record_vector {};
};
