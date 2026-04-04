#include "game/sim/population.h"

#include <math.h>
#include <string.h>

#include "jsk.h"
#include "jsk_log.h"

#include "game/sim/sim_data.h"
#include "game/sim/physics.h"

const char *ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE( DEFINE_LIST_STRING )
};

const char *NATION_TYPE_STRING[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_STRING )
};

i32 population_nation_type_index_from_string(const char *nation_type_string)
{
    for (i32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        if (strcmp(nation_type_string, NATION_TYPE_STRING[nation_type_index]) == 0)
        {
            return nation_type_index;
        }
    }

    return -1;
}

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

static void init_nations(Population* population)
{
    const f32 nation_offset = 32.0f;
    
    Nation *wolf_nation = &population->nation_array[NATION_TYPE_WOLF];
    
    wolf_nation->nation_type = NATION_TYPE_WOLF;

    wolf_nation->home_coordinate[0] = WORLD_CENTER_F32 + nation_offset;
    wolf_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    wolf_nation->home_coordinate[2] = TOWER_ROOF_Z + 5.0f;

    Nation *eagle_nation = &population->nation_array[NATION_TYPE_EAGLE];
    
    eagle_nation->nation_type = NATION_TYPE_EAGLE;

    eagle_nation->home_coordinate[0] = WORLD_CENTER_F32 - nation_offset;
    eagle_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    eagle_nation->home_coordinate[2] = TOWER_ROOF_Z + 5.0f;

    Nation *lion_nation = &population->nation_array[NATION_TYPE_LION];

    lion_nation->nation_type = NATION_TYPE_LION;

    lion_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    lion_nation->home_coordinate[1] = WORLD_CENTER_F32 + nation_offset;
    lion_nation->home_coordinate[2] = TOWER_ROOF_Z + 5.0f;

    Nation *horse_nation = &population->nation_array[NATION_TYPE_HORSE];
    
    horse_nation->nation_type = NATION_TYPE_HORSE;

    horse_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    horse_nation->home_coordinate[1] = WORLD_CENTER_F32 - nation_offset;
    horse_nation->home_coordinate[2] = TOWER_ROOF_Z + 1.0f;
}

static void init_judge(Population *population)
{
    Actor judge;
    judge.actor_type = ACTOR_TYPE_JUDGE;
    judge.movement_type = MOVEMENT_TYPE_GROUND;
    judge.nation_type = NATION_TYPE_LION;

    judge.box_collider.collision_enabled = true;
    
    judge.box_collider.radius[0] = 0.4f;
    judge.box_collider.radius[1] = 0.4f;
    judge.box_collider.radius[2] = 0.9f;
    
    judge.position[0] = WORLD_CENTER_F32;
    judge.position[1] = WORLD_CENTER_F32 - 12.0f;
    judge.position[2] = TOWER_ROOF_Z + 4.0f;

    judge.rotation[0] = 0.0f;
    judge.rotation[1] = 0.0f;
    judge.rotation[2] = 90.0f;

    judge.speed = JUDGE_DEFAULT_GROUND_SPEED;

    judge.velocity[0] = 0.0f;
    judge.velocity[1] = 0.0f;
    judge.velocity[2] = 0.0f;

    judge.is_grounded = false;

    judge.actor_control.active = false;
    judge.actor_control.decision_clock = 0;
    judge.actor_control.decision_period = 0;

    population->judge_handle = add_actor(population, &judge);

    LOG_INFO("Generated Judge, Index: %i", population->judge_handle.index);
}

static void init_agents(Population* population)
{
    for (i32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (i32 agent_index = 0; agent_index < AGENT_INITIAL_POPULATION; ++agent_index)
        {
            Actor agent;
            agent.actor_type = ACTOR_TYPE_AGENT;
            agent.movement_type = MOVEMENT_TYPE_GROUND;
            agent.nation_type = (NationType)(rand() % NATION_TYPE_COUNT);

            agent.box_collider.collision_enabled = true;
    
            agent.box_collider.radius[0] = 0.4f;
            agent.box_collider.radius[1] = 0.4f;
            agent.box_collider.radius[2] = 0.9f;

            const Nation *nation = &population->nation_array[agent.nation_type];
            
            agent.position[0] = nation->home_coordinate[0] - 10 + rand() % 20;
            agent.position[1] = nation->home_coordinate[1] - 10 + rand() % 20;
            agent.position[2] = nation->home_coordinate[2] + 30;

            agent.rotation[0] = 0.0f;
            agent.rotation[1] = 0.0f;
            agent.rotation[2] = rand() % 360;
            
            agent.speed = AGENT_DEFAULT_GROUND_SPEED;

            agent.velocity[0] = 0.0f;
            agent.velocity[1] = 0.0f;
            agent.velocity[2] = 0.0f;

            agent.is_grounded = false;

            agent.actor_control.active = true;
            agent.actor_control.decision_clock = rand() % 500;
            agent.actor_control.decision_period = 500;

            const ActorHandle agent_handle = add_actor(population, &agent);

            LOG_INFO(
                "Generated %s Agent, Index: %i at %f %f %f",
                NATION_TYPE_STRING[agent.nation_type],
                agent_handle.index,
                agent.position[0], agent.position[1], agent.position[2]
            );
        }
    }
}

static void update_actor(Sim *sim, Actor *actor)
{
    if (actor->actor_control.active)
    {
        if (actor->actor_control.decision_clock < actor->actor_control.decision_period)
        {
            actor->actor_control.decision_clock++;
        }
        else
        {
            const i32 direction_angle = rand() % 360;
            const vec2 direction = { cosf(glm_rad(direction_angle)), sinf(glm_rad(direction_angle)) };

            actor->velocity[0] = direction[0] * AGENT_DEFAULT_GROUND_SPEED;
            actor->velocity[1] = direction[1] * AGENT_DEFAULT_GROUND_SPEED;

            actor->rotation[2] = direction_angle;
            
            actor->actor_control.decision_clock = 0;
        }
    }
    
    switch (actor->movement_type)
    {
    case MOVEMENT_TYPE_GROUND: physics_integrate(sim, actor); break;
    case MOVEMENT_TYPE_DEBUG: physics_integrate(sim, actor); break;
    default: break;
    }
}

static void update_actors(Sim *sim)
{   
    for (i32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        if (sim->population.actor_pool.generation_array[actor_index] == 0)
        {
            continue;
        }
        
        Actor *actor = &sim->population.actor_pool.actor_array[actor_index];

        update_actor(sim, actor);
    }
}

void population_init(Sim *sim)
{
    Population *population = &sim->population;
    
    init_actor_pool(population);

    init_nations(population);
    
    init_judge(population);
    init_agents(population);
}

void population_update(Sim *sim)
{
    update_actors(sim);
}

void population_close(Sim *sim)
{

}
