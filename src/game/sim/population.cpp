#include "game/sim/population.h"

#include <string.h>

#include "justsky.h"
#include "justsky_log.h"

#include "game/sim/actor.h"
#include "game/sim/nation.h"
#include "game/sim/physics.h"
#include "game/sim/work.h"
#include "game/sim/world.h"

static void init_judge(Population *population)
{
    Actor judge = {
        .actor_type = ACTOR_TYPE_JUDGE,
        .nation_type = NATION_TYPE_LION,
        .movement_type = MOVEMENT_TYPE_GROUND,
        .position = { WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f },
        .rotation = { 0.0f, 0.0f, 90.0f },
        .rotation_target = { 0.0f, 0.0f, 90.0f },
        .is_grounded = false,
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
        .velocity = { 0.0f, 0.0f, 0.0f },
        .box_collider = {
            .collision_enabled = true,
            .radius = { 0.30f, 0.30f, 0.90f },
        },
    };

    actor_add(&population->actor_pool, &judge);
    
    population->judge_id = judge.actor_id;

    LOG_INFO("Generated Judge, ID: %i", population->judge_id);
}

static void init_agents(Population* population, Work *work)
{
    for (i32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (i32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const NationType nation_type = (NationType)(rand() % NATION_TYPE_COUNT);
            const Nation *nation = &population->nation_array[nation_type];

            const vec3 position = {
                (f32)nation->home_coordinate[0] - 6 + rand() % 12,
                (f32)nation->home_coordinate[1] - 6 + rand() % 12,
                (f32)nation->home_coordinate[2] + 4,
            };
            
            const vec3 rotation = { 0.0f, 0.0f, (f32)(rand() % 360) };
            
            Actor agent = {
                .actor_type = ACTOR_TYPE_AGENT,
                .nation_type = nation_type,
                .movement_type = MOVEMENT_TYPE_GROUND,
                .position = { position[0], position[1], position[2] },
                .rotation = { rotation[0], rotation[1], rotation[2] },
                .rotation_target = { rotation[0], rotation[1], rotation[2] },
                .is_grounded = false,
                .speed = AGENT_DEFAULT_GROUND_SPEED,
                .velocity = { 0.0f, 0.0f, 0.0f },
                .box_collider = {
                    .collision_enabled = true,
                    .radius = { 0.40f, 0.40f, 0.90f },
                },
            };
            
            actor_add(&population->actor_pool, &agent);

            ActState act_state = {
                .wander = {
                    .tick = (u32)(rand() % 500),
                    .tick_limit = 500,
                }
            };
            
            work_add_act(work, &agent, ACT_TYPE_WANDER, act_state);

            LOG_INFO(
                "Generated %s Agent, ID: %i at (%.1f %.1f %.1f)",
                NATION_TYPE_STRING[agent.nation_type],
                agent.actor_id,
                agent.position[0], agent.position[1], agent.position[2]
            );
        }
    }
}

void init_nations(Population *population)
{
    constexpr f32 nation_offset = 76.0f;
    
    Nation *wolf_nation = &population->nation_array[NATION_TYPE_WOLF];
    
    wolf_nation->nation_type = NATION_TYPE_WOLF;

    wolf_nation->home_coordinate[0] = (i32)(WORLD_CENTER_F32 + nation_offset);
    wolf_nation->home_coordinate[1] = (i32)(WORLD_CENTER_F32 + 0.0f);
    wolf_nation->home_coordinate[2] = (i32)(ROOF_Z + 3.0f);

    Nation *eagle_nation = &population->nation_array[NATION_TYPE_EAGLE];
    
    eagle_nation->nation_type = NATION_TYPE_EAGLE;

    eagle_nation->home_coordinate[0] = (i32)(WORLD_CENTER_F32 - nation_offset);
    eagle_nation->home_coordinate[1] = (i32)(WORLD_CENTER_F32 + 0.0f);
    eagle_nation->home_coordinate[2] = (i32)(ROOF_Z + 3.0f);

    Nation *bear_nation = &population->nation_array[NATION_TYPE_BEAR];
    
    bear_nation->nation_type = NATION_TYPE_BEAR;

    bear_nation->home_coordinate[0] = (i32)(WORLD_CENTER_F32 + 0.0f);
    bear_nation->home_coordinate[1] = (i32)(WORLD_CENTER_F32 + nation_offset);
    bear_nation->home_coordinate[2] = (i32)(ROOF_Z + 1.0f);
    
    Nation *lion_nation = &population->nation_array[NATION_TYPE_LION];

    lion_nation->nation_type = NATION_TYPE_LION;

    lion_nation->home_coordinate[0] = (i32)(WORLD_CENTER_F32 + 0.0f);
    lion_nation->home_coordinate[1] = (i32)(WORLD_CENTER_F32 - nation_offset);
    lion_nation->home_coordinate[2] = (i32)(ROOF_Z + 3.0f);
}

static void init_actor_pool(Population *population)
{
    population->actor_pool.free_count = ACTOR_MAX;
    population->actor_pool.active_count = 0;

    for (PoolID pool_id = 0; pool_id < ACTOR_MAX; ++pool_id)
    {
        population->actor_pool.active_array[pool_id] = 0;
        population->actor_pool.free_array[pool_id] = pool_id;
    }
}

void population_init(Population *population, Work *work)
{
    init_nations(population);

    init_actor_pool(population);
    
    init_judge(population);
    init_agents(population, work);
}

void population_close()
{

}
