#pragma once

#include <functional>

#include "actor.h"
#include "constants.h"
#include "control.h"
#include "core/random.h"
#include "core/types.h"

class Population;
class World;

using JobCallback = std::function<void(World&, Population&)>;

struct JobRecord
{
    s32 frequency   { 1 };
    s32 phase       { 0 };

    JobCallback job_callback {};
};

class Work
{
public:
    void init(Population& population);
    void update(World& world, Population& population);

    void add_job(s32 frequency, s32 phase, JobCallback job_callback);

    Random random { WORK_SEED };

private:
    u64 tick_count { 0 };

    std::vector<JobRecord> task_record_vector {};
};
