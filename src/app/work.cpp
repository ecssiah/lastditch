#include "work.h"

#include <iostream>
#include <ostream>

#include "actor.h"
#include "app.h"
#include "population.h"

using namespace std;

void
Work::update(World& world, Population& population)
{
    ++tick_count;

    for (TaskRecord& task_record : task_record_vector)
    {
        if ((tick_count + task_record.phase) % task_record.frequency == 0)
        {
            task_record.task();
        }
    }
}

void
Work::schedule(const s32 frequency, const s32 phase, std::function<void()> task)
{
    const TaskRecord task_record {
        .frequency = frequency,
        .phase = phase,
        .task = std::move(task),
    };

    task_record_vector.push_back(task_record);
}