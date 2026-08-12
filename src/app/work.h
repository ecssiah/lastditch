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

using JobCallback = std::function<void(World&, Population&)>;

struct Job
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    JobCallback job_callback {};
};

extern const std::vector<Job> JOB_VECTOR;

class Work
{
public:
    void update(World& world, Population& population);

    b32 is_due(const Job& job) const;

    Random random { WORK_SEED };
    Navigation navigation {};

private:
    u64 tick_count { 0 };

    std::vector<Job> job_vector;
};
