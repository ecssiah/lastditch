#include "game/sim/population.h"

#include "core/types.h"
#include "core/log.h"
#include "game/sim/actor.h"
#include "game/sim/nation.h"
#include "game/sim/physics.h"
#include "game/sim/work.h"
#include "game/sim/world.h"

static void 
init_judge(Population& population)
{
    Actor judge = {
        .actor_type = ActorType::judge,
        .nation_type = NationType::lion,
        .movement_type = MovementType::ground,
        .position = {WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f},
        .rotation = {0.0f, 0.0f, 90.0f},
        .rotation_target = {0.0f, 0.0f, 90.0f},
        .is_grounded = false,
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
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
        NATION_TYPE_STRING[static_cast<u8>(judge.nation_type)],
        population.judge_id,
        judge.position.x, 
        judge.position.y, 
        judge.position.z
    );
}

static void 
init_agents(Population& population, Work& work)
{
    for (i32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (i32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const i32 nation_type_index = rand() % NATION_TYPE_COUNT;
            
            const NationType nation_type = static_cast<NationType>(nation_type_index);
            const Nation& nation = population.nation_array[nation_type_index];

            const vec3 position = {
                static_cast<f32>(nation.home_coordinate.x) - 6 + rand() % 12,
                static_cast<f32>(nation.home_coordinate.y) - 6 + rand() % 12,
                static_cast<f32>(nation.home_coordinate.z) + 4,
            };

            const vec3 rotation = {
                0.0f, 
                0.0f, 
                static_cast<f32>(rand() % 360)
            };

            Actor agent = {
                .actor_type = ActorType::agent,
                .nation_type = nation_type,
                .movement_type = MovementType::ground,
                .position = position,
                .rotation = rotation,
                .rotation_target = rotation,
                .is_grounded = false,
                .speed = AGENT_DEFAULT_GROUND_SPEED,
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
                NATION_TYPE_STRING[nation_type_index],
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

    Nation& wolf_nation = population.nation_array[static_cast<u8>(NationType::wolf)];

    wolf_nation.nation_type = NationType::wolf;

    wolf_nation.home_coordinate.x = WORLD_CENTER_F32 + nation_offset;
    wolf_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    wolf_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& eagle_nation = population.nation_array[static_cast<u8>(NationType::eagle)];

    eagle_nation.nation_type = NationType::eagle;

    eagle_nation.home_coordinate.x = WORLD_CENTER_F32 - nation_offset;
    eagle_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    eagle_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& bear_nation = population.nation_array[static_cast<u8>(NationType::bear)];

    bear_nation.nation_type = NationType::bear;

    bear_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    bear_nation.home_coordinate.y = WORLD_CENTER_F32 + nation_offset;
    bear_nation.home_coordinate.z = ROOF_Z + 1.0f;

    Nation& lion_nation = population.nation_array[static_cast<u8>(NationType::lion)];

    lion_nation.nation_type = NationType::lion;

    lion_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    lion_nation.home_coordinate.y = WORLD_CENTER_F32 - nation_offset;
    lion_nation.home_coordinate.z = ROOF_Z + 3.0f;
}

static void 
init_actor_pool(Population& population)
{
    population.actor_pool.free_count = ACTOR_MAX;
    population.actor_pool.active_count = 0;

    for (PoolID pool_id = 0; pool_id < ACTOR_MAX; ++pool_id)
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
population_close()
{
}
