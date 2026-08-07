#include "work.h"

#include <algorithm>

#include "actor.h"
#include "population.h"

using namespace std;

void
Work::init(Population &population)
{
    add_job(
        10,
        0,
        [](World&, Population& population)
        {
            for (Actor& actor : population.actor_vector)
            {
                if (actor.decision_timer > 0)
                {
                    --actor.decision_timer;
                }
                else
                {
                    actor.decision_timer = population.random.uniform(10, 50);

                    const f32 distance_to_target { actor.rotation_target.z - actor.rotation.z };

                    if (abs(distance_to_target) < 1.0f)
                    {
                        const b32 should_act { population.random.uniform(0 ,1) == 1 };

                        if (should_act)
                        {
                            actor.rotation_target.z = clamp(
                                actor.rotation_target.z + population.random.uniform(-120.0f, 120.0f),
                                0.0f,
                                360.0f
                            );

                            const Vec2 direction { direction_from_angle(actor.rotation_target.z) };

                            actor.velocity = {
                                direction.x * actor.move_speed,
                                direction.y * actor.move_speed,
                                actor.velocity.z,
                            };
                        }
                        else
                        {
                            actor.velocity = {
                                0.0f,
                                0.0f,
                                actor.velocity.z,
                            };
                        }
                    }
                }
            }
        }
    );
}

void
Work::update(World& world, Population& population)
{
    ++tick_count;

    for (JobRecord& task_record : task_record_vector)
    {
        if ((tick_count + task_record.phase) % task_record.frequency == 0)
        {
            task_record.job_callback(world, population);
        }
    }
}

void
Work::add_job(const s32 frequency, const s32 phase, JobCallback job_callback)
{
    const JobRecord task_record {
        .frequency = frequency,
        .phase = phase,
        .job_callback = std::move(job_callback),
    };

    task_record_vector.push_back(task_record);
}
