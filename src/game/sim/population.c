#include "game/sim/population.h"

#include <string.h>

#include "game/sim/actor.h"
#include "game/sim/nation.h"
#include "justsky.h"
#include "justsky_log.h"

#include "game/sim/physics.h"
#include "game/sim/world.h"

static void init_judge(Population *population)
{
    Actor judge = {
        .actor_type = ACTOR_TYPE_JUDGE,
        .movement_type = MOVEMENT_TYPE_GROUND,
        .nation_type = NATION_TYPE_LION,
        .position = {
            WORLD_CENTER_F32,
            WORLD_CENTER_F32 - 12.0f,
            ROOF_Z + 4.0f,
        },
        .rotation = { 0.0f, 0.0f, 90.0f },
        .rotation_target = { 0.0f, 0.0f, 90.0f },
        .velocity = { 0.0f, 0.0f, 0.0f },
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
        .is_grounded = false,
        .box_collider = {
            .collision_enabled = true,
            .radius = { 0.30f, 0.30f, 0.90f },
        },
        .actor_control = {
            .control_type = CONTROL_TYPE_NONE,
            .decision_clock = 0,
            .decision_period = 0,
        },
    };

    population->judge_handle = actor_add(&population->actor_pool, &judge);

    LOG_INFO("Generated Judge, Index: %i", population->judge_handle.index);
}

static void init_agents(Population* population)
{
    for (u32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (u32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const NationType nation_type = (NationType)(rand() % NATION_TYPE_COUNT);
            const Nation *nation = &population->nation_array[nation_type];

            const vec3 rotation = { 0.0f, 0.0f, rand() % 360 };
            
            Actor agent = {
                .actor_type = ACTOR_TYPE_AGENT,
                .movement_type = MOVEMENT_TYPE_GROUND,
                .nation_type = nation_type,
                .position = {
                    nation->home_coordinate[0] - 6 + rand() % 12,
                    nation->home_coordinate[1] - 6 + rand() % 12,
                    nation->home_coordinate[2] + 4,
                },
                .rotation = { rotation[0], rotation[1], rotation[2] },
                .rotation_target = { rotation[0], rotation[1], rotation[2] },
                .velocity = { 0.0f, 0.0f, 0.0f },
                .speed = AGENT_DEFAULT_GROUND_SPEED,
                .is_grounded = false,
                .box_collider = {
                    .collision_enabled = true,
                    .radius = { 0.40f, 0.40f, 0.90f },
                },
                .actor_control = {
                    .control_type = CONTROL_TYPE_WANDER,
                    .decision_clock = rand() % 500,
                    .decision_period = 500,
                },
            };
            
            const ActorHandle agent_handle = actor_add(&population->actor_pool, &agent);

            LOG_INFO(
                "Generated %s Agent, Index: %i at (%.1f %.1f %.1f)",
                NATION_TYPE_STRING[agent.nation_type],
                agent_handle.index,
                agent.position[0], agent.position[1], agent.position[2]
            );
        }
    }
}

void population_init(Population *population)
{
    nation_init(population->nation_array);
    
    actor_init_pool(&population->actor_pool);
    
    init_judge(population);
    init_agents(population);
}

void population_update(Population *population, World *world)
{
    ActorPool *actor_pool = &population->actor_pool;
    
    for (u32 pool_index = 0; pool_index < actor_pool->active_count; ++pool_index)
    {
        const u32 actor_index = actor_pool->active_array[pool_index];
        Actor *actor = &population->actor_pool.actor_array[actor_index];

        actor_control(world, actor);
    }
}

void population_close()
{

}
