#include "game/sim/population.h"

#include <string.h>

#include "game/sim/sim_data.h"
#include "jsk.h"
#include "jsk_log.h"

const char *ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE( DEFINE_LIST_STRING )
};

i32 population_actor_type_index_from_string(const char *actor_type_string)
{
    for (i32 actor_type_index = 0; actor_type_index < ACTOR_TYPE_COUNT; ++actor_type_index)
    {
        if (strcmp(actor_type_string, ACTOR_TYPE_STRING[actor_type_index]) == 0)
        {
            return actor_type_index;
        }
    }

    return -1;
}

static ActorHandle add_actor(Population *population, Actor *actor)
{
    ActorPool *actor_pool = &population->actor_pool;
    
    const u32 actor_index = actor_pool->free_array[--actor_pool->free_count];
    const u32 actor_generation = ++actor_pool->generation_array[actor_index];
    
    actor_pool->actor_array[actor_index] = *actor;
    
    actor_pool->active_array[actor_index] = actor_pool->active_count;
    actor_pool->active_array[actor_pool->active_count++] = actor_index;

    assert(actor_pool->free_count + actor_pool->active_count == ACTOR_MAX);

    ActorHandle actor_handle = { actor_index, actor_generation };
    
    return actor_handle;
}

static void init_actor_pool(Population *population)
{
    population->actor_pool.free_count = ACTOR_MAX;
    population->actor_pool.active_count = 0;

    for (i32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        population->actor_pool.free_array[actor_index] = actor_index;
        population->actor_pool.generation_array[actor_index] = 0;
    }
}

static void init_judge(Population *population)
{
    Actor judge;
    judge.actor_type = ACTOR_TYPE_JUDGE;
    judge.movement_type = MOVEMENT_TYPE_DEBUG;

    judge.box_collider.collision_enabled = false;
    
    judge.box_collider.radius[0] = 0.4f;
    judge.box_collider.radius[1] = 0.4f;
    judge.box_collider.radius[2] = 0.9f;
    
    judge.position[0] = WORLD_CENTER_F32;
    judge.position[1] = WORLD_CENTER_F32 - 12;
    judge.position[2] = TOWER_ROOF_Z + 5;

    judge.rotation[0] = 0.0f;
    judge.rotation[1] = 0.0f;
    judge.rotation[2] = 90.0f;

    judge.speed = JUDGE_DEFAULT_DEBUG_SPEED;

    judge.velocity[0] = 0.0f;
    judge.velocity[1] = 0.0f;
    judge.velocity[2] = 0.0f;

    judge.is_grounded = false;

    population->judge_handle = add_actor(population, &judge);

    LOG_INFO("Generated Judge, Index: %i", population->judge_handle.index);
}

static void init_agents(Population* population)
{
    for (i32 agent_index = 0; agent_index < AGENT_INITIAL_POPULATION; ++agent_index)
    {
        Actor agent;
        agent.actor_type = ACTOR_TYPE_AGENT;
        agent.movement_type = MOVEMENT_TYPE_GROUND;

        agent.box_collider.collision_enabled = true;
    
        agent.box_collider.radius[0] = 0.4f;
        agent.box_collider.radius[1] = 0.4f;
        agent.box_collider.radius[2] = 0.9f;

        agent.position[0] = WORLD_CENTER_F32 - 10 + rand() % 20;
        agent.position[1] = WORLD_CENTER_F32 - 10 + rand() % 20;
        agent.position[2] = TOWER_ROOF_Z + 5;

        agent.rotation[0] = 0.0f;
        agent.rotation[1] = 0.0f;
        agent.rotation[2] = rand() % 360;

        agent.speed = AGENT_DEFAULT_GROUND_SPEED;

        agent.velocity[0] = 0.0f;
        agent.velocity[1] = 0.0f;
        agent.velocity[2] = 0.0f;

        agent.is_grounded = false;

        ActorHandle agent_handle = add_actor(population, &agent);

        LOG_INFO("Generated Agent, Index: %i at %f %f %f", agent_handle.index, agent.position[0], agent.position[1], agent.position[2]);
    }
}

void population_init(Sim *sim)
{
    Population *population = &sim->population;
    
    init_actor_pool(population);
    init_judge(population);
    init_agents(population);
}

void population_update(Sim *sim)
{

}

void population_close(Sim *sim)
{

}
