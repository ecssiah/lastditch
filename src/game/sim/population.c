#include "game/sim/population.h"

#include <string.h>

#include "justsky.h"
#include "justsky_log.h"

#include "game/sim/actor.h"
#include "game/sim/nation.h"
#include "game/sim/physics.h"
#include "game/sim/scheduler.h"
#include "game/sim/world.h"

static void init_judge(Population *population)
{
    Actor judge = {
        .actor_type = ACTOR_TYPE_JUDGE,
        .movement_type = MOVEMENT_TYPE_GROUND,
        .nation_type = NATION_TYPE_LION,
        .position = { WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f },
        .rotation = { 0.0f, 0.0f, 90.0f },
        .rotation_target = { 0.0f, 0.0f, 90.0f },
        .velocity = { 0.0f, 0.0f, 0.0f },
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
        .is_grounded = false,
        .box_collider = {
            .collision_enabled = true,
            .radius = { 0.30f, 0.30f, 0.90f },
        },
    };

    actor_add(&population->actor_pool, &judge);
    
    population->judge_id = judge.actor_id;

    LOG_INFO("Generated Judge, ID: %i", population->judge_id);
}

static void init_agents(Population* population, Scheduler *scheduler)
{
    for (u32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (u32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const NationType nation_type = (NationType)(rand() % NATION_TYPE_COUNT);
            const Nation *nation = &population->nation_array[nation_type];

            const vec3 position = {
                nation->home_coordinate[0] - 6 + rand() % 12,
                nation->home_coordinate[1] - 6 + rand() % 12,
                nation->home_coordinate[2] + 4,
            };
            
            const vec3 rotation = { 0.0f, 0.0f, rand() % 360 };
            
            Actor agent = {
                .actor_type = ACTOR_TYPE_AGENT,
                .movement_type = MOVEMENT_TYPE_GROUND,
                .nation_type = nation_type,
                .position = { position[0], position[1], position[2] },
                .rotation = { rotation[0], rotation[1], rotation[2] },
                .rotation_target = { rotation[0], rotation[1], rotation[2] },
                .velocity = { 0.0f, 0.0f, 0.0f },
                .speed = AGENT_DEFAULT_GROUND_SPEED,
                .is_grounded = false,
                .box_collider = {
                    .collision_enabled = true,
                    .radius = { 0.40f, 0.40f, 0.90f },
                },
            };
            
            actor_add(&population->actor_pool, &agent);

            BehaviorState behavior_state = {
                .wander = {
                    .tick = rand() % 500,
                    .tick_limit = 500,
                }
            };
            
            scheduler_add_behavior(scheduler, &agent, wander_run, behavior_state);

            LOG_INFO(
                "Generated %s Agent, ID: %i at (%.1f %.1f %.1f)",
                NATION_TYPE_STRING[agent.nation_type],
                agent.actor_id,
                agent.position[0], agent.position[1], agent.position[2]
            );
        }
    }
}

void population_init(Population *population, Scheduler *scheduler)
{
    nation_init(population->nation_array);
    
    actor_init_pool(&population->actor_pool);
    
    init_judge(population);
    init_agents(population, scheduler);
}

void population_update(Population *population, World *world)
{
    ActorPool *actor_pool = &population->actor_pool;
    
    for (PoolID pool_id = 0; pool_id < actor_pool->active_count; ++pool_id)
    {
        const ActorID actor_id = actor_pool->active_array[pool_id];
        Actor *actor = &population->actor_pool.actor_array[actor_id];
    }
}

void population_close()
{

}
