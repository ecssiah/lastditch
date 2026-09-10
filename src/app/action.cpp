#include <algorithm>
#include <cmath>
#include "action.h"
#include "population.h"
#include "world.h"
#include "core/geometry/interpolation.h"
#include "data/actor.h"
#include "data/work.h"

using namespace std;

const vector<Work> WORK_VECTOR
{
    {
        .frequency = 4,
        .phase = 0,
        .work_function = [](World&, Population& population)
        {
            for (Actor& actor : population.actor_vector)
            {
                if (actor.id == population.judge_id)
                {
                    continue;
                }

                if (actor.decision_timer > 0)
                {
                    --actor.decision_timer;
                }
                else
                {
                    actor.decision_timer = population.random.uniform(10, 50);

                    const f32 distance_to_target
                    {
                        get_shortest_angle_delta(actor.movement_yaw, actor.rotation_target.z)
                    };

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

                            const Vec2 direction { World::get_direction_from_angle(actor.rotation_target.z) };

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
        },
    },
    {
        .frequency = 1,
        .phase = 0,
        .work_function = [](World& world, Population& population)
        {
            for (Actor& actor : population.actor_vector)
            {
                const f32 alpha
                {
                    1.0f - std::exp(-actor.turn_speed * FIXED_FRAME_TIME_32)
                };

                actor.movement_yaw = interpolate_angle_to(
                    actor.movement_yaw,
                    actor.rotation_target.z,
                    actor.turn_speed,
                    FIXED_FRAME_TIME_32
                );

                actor.orientation = spherical_interpolation_to(
                    actor.orientation,
                    Quaternion::from_euler_degrees(
                        actor.rotation_target.x,
                        actor.rotation_target.y,
                        actor.rotation_target.z
                    ),
                    alpha
                );

                Physics::update_actor(world, actor);
            }
        },
    },
};

void
Action::init(World& world)
{
    navigation.init(world);
}

void
Action::update(World& world, Population& population)
{
    ++tick_count;

    for (const Work& work : WORK_VECTOR)
    {
        if (is_due(work))
        {
            work.work_function(world, population);
        }
    }
}

b32
Action::is_due(const Work &work) const
{
    return (tick_count + work.phase) % work.frequency == 0;
}
