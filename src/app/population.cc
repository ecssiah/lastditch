#include "app/population.h"

#include "core/types.h"
#include "core/log.h"
#include "app/actor.h"
#include "app/nation.h"
#include "app/physics.h"
#include "app/work.h"
#include "app/world.h"

static void 
init_judge(Population& population)
{
    Actor judge = {
        .actor_type = ActorType::Judge,
        .nation_type = NationType::Lion,
        .movement_type = MovementType::Ground,
        .position = {world_center_f32, world_center_f32 - 12.0f, roof_z + 4.0f},
        .rotation = {0.0f, 0.0f, 90.0f},
        .rotation_target = {0.0f, 0.0f, 90.0f},
        .is_grounded = false,
        .speed = judge_default_ground_speed,
        .velocity = {0.0f, 0.0f, 0.0f},
        .box_collider = {
            .collision_enabled = true,
            .radius = {0.30f, 0.30f, 0.90f},
        },
    };

    actor_add(population.actor_pool, judge);

    population.judge_id = judge.actor_id;

    LOG_INFO(
        "Generated %s judge, ID: %i, at (%.1f %.1f %.1f)", 
        nation_type_string_array[static_cast<u8>(judge.nation_type)],
        population.judge_id,
        judge.position.x, 
        judge.position.y, 
        judge.position.z
    );
}

static void 
init_agents(Population& population, Work& work)
{
    for (i32 nation_index = 0; nation_index < nation_type_count; ++nation_index)
    {
        for (i32 agent_index = 0; agent_index < agent_initial_count; ++agent_index)
        {
            const i32 nation_type_index = rand() % nation_type_count;
            
            const NationType nation_type = static_cast<NationType>(nation_type_index);
            const Nation& nation = population.nation_array[nation_type_index];

            const Vec3 position = {
                static_cast<f32>(nation.home_coordinate.x) - 6 + rand() % 12,
                static_cast<f32>(nation.home_coordinate.y) - 6 + rand() % 12,
                static_cast<f32>(nation.home_coordinate.z) + 4,
            };

            const Vec3 rotation = {
                0.0f, 
                0.0f, 
                static_cast<f32>(rand() % 360)
            };

            Actor agent = {
                .actor_type = ActorType::Agent,
                .nation_type = nation_type,
                .movement_type = MovementType::Ground,
                .position = position,
                .rotation = rotation,
                .rotation_target = rotation,
                .is_grounded = false,
                .speed = agent_default_ground_speed,
                .velocity = vec3_broadcast(0.0f),
                .box_collider = {
                    .collision_enabled = true,
                    .radius = {0.40f, 0.40f, 0.90f},
                },
            };

            actor_add(population.actor_pool, agent);

            const ActState act_state = {
                .wander = {
                    .tick = rand() % 500,
                    .tick_limit = 500,
                }
            };

            work_add_act(work, agent, ActType::wander, act_state);

            LOG_INFO(
                "Generated %s agent, ID: %i, at (%.1f %.1f %.1f)",
                nation_type_string_array[nation_type_index],
                agent.actor_id,
                agent.position.x, 
                agent.position.y, 
                agent.position.z
            );
        }
    }
}

void 
init_nations(Population& population)
{
    constexpr f32 nation_offset = 76.0f;

    Nation& wolf_nation = population.nation_array[static_cast<u8>(NationType::Wolf)];

    wolf_nation.nation_type = NationType::Wolf;

    wolf_nation.home_coordinate.x = world_center_f32 + nation_offset;
    wolf_nation.home_coordinate.y = world_center_f32 + 0.0f;
    wolf_nation.home_coordinate.z = roof_z + 3.0f;

    Nation& eagle_nation = population.nation_array[static_cast<u8>(NationType::Eagle)];

    eagle_nation.nation_type = NationType::Eagle;

    eagle_nation.home_coordinate.x = world_center_f32 - nation_offset;
    eagle_nation.home_coordinate.y = world_center_f32 + 0.0f;
    eagle_nation.home_coordinate.z = roof_z + 3.0f;

    Nation& bear_nation = population.nation_array[static_cast<u8>(NationType::Bear)];

    bear_nation.nation_type = NationType::Bear;

    bear_nation.home_coordinate.x = world_center_f32 + 0.0f;
    bear_nation.home_coordinate.y = world_center_f32 + nation_offset;
    bear_nation.home_coordinate.z = roof_z + 1.0f;

    Nation& lion_nation = population.nation_array[static_cast<u8>(NationType::Lion)];

    lion_nation.nation_type = NationType::Lion;

    lion_nation.home_coordinate.x = world_center_f32 + 0.0f;
    lion_nation.home_coordinate.y = world_center_f32 - nation_offset;
    lion_nation.home_coordinate.z = roof_z + 3.0f;
}

static void 
init_actor_pool(Population& population)
{
    population.actor_pool.free_count = actor_max;
    population.actor_pool.active_count = 0;

    for (i32 pool_id = 0; pool_id < actor_max; ++pool_id)
    {
        population.actor_pool.active_array[pool_id] = 0;
        population.actor_pool.free_array[pool_id] = pool_id;
    }
}

void 
population_init(Population& population, Work& work)
{
    init_nations(population);

    init_actor_pool(population);

    init_judge(population);
    init_agents(population, work);
}

void 
population_quit()
{
}
