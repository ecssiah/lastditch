#include "app/population.h"

#include <cassert>
#include "core/types.h"
#include "core/log.h"
#include "app/actor.h"
#include "app/nation.h"
#include "app/physics.h"
#include "app/work.h"
#include "app/world.h"

Population::Population()
    :
    random{POPULATION_SEED},
    nation_array{},
    judge_id{}
{

}

void
Population::init(Work& work)
{
    init_nations();
    init_judge();
    init_agents(work);
}

void
Population::quit()
{

}

Actor&
Population::get_judge()
{
    return actor_pool.get(judge_id);
}

const Actor&
Population::get_judge() const
{
    return actor_pool.get(judge_id);
}

Actor&
Population::get_actor(s32 actor_id)
{
    return actor_pool.get(actor_id);
}

const Actor&
Population::get_actor(s32 actor_id) const
{
    return actor_pool.get(actor_id);
}

ActorPool&
Population::get_actor_pool()
{
    return actor_pool;
}

void
Population::init_judge()
{
    Actor judge{
        .actor_type = ActorType::Judge,
        .nation_type = NationType::Lion,
        .position = {WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f},
        .rotation = {0.0f, 0.0f, 90.0f},
        .rotation_target = {0.0f, 0.0f, 90.0f},
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
        .velocity = {0.0f, 0.0f, 0.0f},
        .box_collider = {
            .collision_enabled = true,
            .radius = {0.30f, 0.30f, 0.90f},
        },
    };

    judge_id = actor_pool.add(judge);

    LOG_INFO(
        "Generated %s judge, ID: %i, at (%.1f %.1f %.1f)", 
        nation_type_string_array[static_cast<u8>(judge.nation_type)],
        judge_id,
        judge.position.x,
        judge.position.y,
        judge.position.z
    );
}

void
Population::init_agents(Work& work)
{
    for (s32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (s32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const s32 nation_type_index {random.uniform(0, NATION_TYPE_COUNT - 1)};
            
            const auto nation_type {static_cast<NationType>(nation_type_index)};
            const Nation& nation {nation_array[nation_type_index]};

            const Vec3 position {
                static_cast<f32>(nation.home_coordinate.x - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.y - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.z + 4),
            };

            const Vec3 rotation {
                0.0f, 
                0.0f, 
                static_cast<f32>(random.uniform(0, 360))
            };

            Actor agent {
                .actor_type = ActorType::Agent,
                .nation_type = nation_type,
                .position = position,
                .rotation = rotation,
                .rotation_target = rotation,
                .speed = AGENT_DEFAULT_GROUND_SPEED,
                .box_collider = {
                    .collision_enabled = true,
                    .radius = {0.40f, 0.40f, 0.90f},
                },
            };

            agent.id = actor_pool.add(agent);

            const TaskState act_state{
                .wander = {
                    .tick = random.uniform(0, 500),
                    .tick_limit = 500,
                },
            };

            Task task {
                .actor_id = agent.id,
                .task_type = TaskType::wander,
                .task_state = act_state,
            };

            task.id = work.get_task_pool().add(task);

            agent.act_id_array[agent.act_id_count++] = task.id;

            LOG_INFO(
                "Generated %s agent, ID: %i, at (%.1f %.1f %.1f)",
                nation_type_string_array[nation_type_index],
                agent.id,
                agent.position.x,
                agent.position.y,
                agent.position.z
            );
        }
    }
}

void 
Population::init_nations()
{
    constexpr f32 nation_offset {76.0f};

    Nation& wolf_nation {nation_array[static_cast<u8>(NationType::Wolf)]};

    wolf_nation.nation_type = NationType::Wolf;

    wolf_nation.home_coordinate.x = WORLD_CENTER_F32 + nation_offset;
    wolf_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    wolf_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& eagle_nation {nation_array[static_cast<u8>(NationType::Eagle)]};

    eagle_nation.nation_type = NationType::Eagle;

    eagle_nation.home_coordinate.x = WORLD_CENTER_F32 - nation_offset;
    eagle_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    eagle_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& bear_nation {nation_array[static_cast<u8>(NationType::Bear)]};

    bear_nation.nation_type = NationType::Bear;

    bear_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    bear_nation.home_coordinate.y = WORLD_CENTER_F32 + nation_offset;
    bear_nation.home_coordinate.z = ROOF_Z + 1.0f;

    Nation& lion_nation {nation_array[static_cast<u8>(NationType::Lion)]};

    lion_nation.nation_type = NationType::Lion;

    lion_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    lion_nation.home_coordinate.y = WORLD_CENTER_F32 - nation_offset;
    lion_nation.home_coordinate.z = ROOF_Z + 3.0f;
}
