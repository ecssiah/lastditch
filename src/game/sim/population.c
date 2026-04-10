#include "game/sim/population.h"

#include <math.h>
#include <string.h>

#include "game/sim/world.h"
#include "jsk.h"
#include "jsk_log.h"

#include "core/math/math.h"
#include "game/sim/physics.h"

const char *ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE( DEFINE_LIST_STRING )
};

const char *NATION_TYPE_STRING[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_STRING )
};

static ActorHandle add_actor(Population *population, Actor *actor)
{
    ActorPool *actor_pool = &population->actor_pool;
    
    const u32 actor_index = actor_pool->free_array[--actor_pool->free_count];
    const u32 actor_generation = ++actor_pool->generation_array[actor_index];
        
    actor_pool->active_array[actor_pool->active_count++] = actor_index;
    actor_pool->actor_array[actor_index] = *actor;
    
    assert(actor_pool->free_count + actor_pool->active_count == ACTOR_MAX);

    ActorHandle actor_handle = { actor_index, actor_generation };
    
    return actor_handle;
}

static void init_actor_pool(Population *population)
{
    population->actor_pool.free_count = ACTOR_MAX;
    population->actor_pool.active_count = 0;

    for (u32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        population->actor_pool.active_array[actor_index] = 0;
        population->actor_pool.generation_array[actor_index] = 0;
        population->actor_pool.free_array[actor_index] = actor_index;
    }
}

static void init_nations(Population* population)
{
    const f32 nation_offset = 76.0f;
    
    Nation *wolf_nation = &population->nation_array[NATION_TYPE_WOLF];
    
    wolf_nation->nation_type = NATION_TYPE_WOLF;

    wolf_nation->home_coordinate[0] = WORLD_CENTER_F32 + nation_offset;
    wolf_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    wolf_nation->home_coordinate[2] = ROOF_Z + 3.0f;

    Nation *eagle_nation = &population->nation_array[NATION_TYPE_EAGLE];
    
    eagle_nation->nation_type = NATION_TYPE_EAGLE;

    eagle_nation->home_coordinate[0] = WORLD_CENTER_F32 - nation_offset;
    eagle_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    eagle_nation->home_coordinate[2] = ROOF_Z + 3.0f;

    Nation *bear_nation = &population->nation_array[NATION_TYPE_BEAR];
    
    bear_nation->nation_type = NATION_TYPE_BEAR;

    bear_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    bear_nation->home_coordinate[1] = WORLD_CENTER_F32 + nation_offset;
    bear_nation->home_coordinate[2] = ROOF_Z + 1.0f;
    
    Nation *lion_nation = &population->nation_array[NATION_TYPE_LION];

    lion_nation->nation_type = NATION_TYPE_LION;

    lion_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    lion_nation->home_coordinate[1] = WORLD_CENTER_F32 - nation_offset;
    lion_nation->home_coordinate[2] = ROOF_Z + 3.0f;
}

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

    population->judge_handle = add_actor(population, &judge);

    LOG_INFO("Generated Judge, Index: %i", population->judge_handle.index);
}

static void init_agents(Population* population)
{
    for (u32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (u32 agent_index = 0; agent_index < AGENT_INITIAL_POPULATION; ++agent_index)
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
            
            const ActorHandle agent_handle = add_actor(population, &agent);

            LOG_INFO(
                "Generated %s Agent, Index: %i at (%.1f %.1f %.1f)",
                NATION_TYPE_STRING[agent.nation_type],
                agent_handle.index,
                agent.position[0], agent.position[1], agent.position[2]
            );
        }
    }
}

static void control_actor(World *world, Actor *actor)
{
    switch (actor->actor_control.control_type)
    {
    case CONTROL_TYPE_WANDER:
    {
        if (actor->actor_control.decision_clock < actor->actor_control.decision_period)
        {
            actor->actor_control.decision_clock++;
        }
        else
        {
            const i32 direction_angle = rand() % 360;

            const vec2 direction = {
                cosf(glm_rad(direction_angle)),
                sinf(glm_rad(direction_angle))
            };

            actor->velocity[0] = direction[0] * AGENT_DEFAULT_GROUND_SPEED;
            actor->velocity[1] = direction[1] * AGENT_DEFAULT_GROUND_SPEED;

            actor->rotation_target[2] = direction_angle;

            actor->actor_control.decision_clock = 0;
        }

        actor->rotation[2] = math_lerp_to(
            actor->rotation[2],
            actor->rotation_target[2],
            8.0f,
            world->delta_time
        );

        break;
    }
    case CONTROL_TYPE_SEEK:
    {
        
        
        break;
    }
    case CONTROL_TYPE_NONE: break;
    default: break;
    }
}

static void update_actor(World *world, Actor *actor)
{
    control_actor(world, actor);
    
    switch (actor->movement_type)
    {
    case MOVEMENT_TYPE_GROUND: physics_integrate(world, actor); break;
    case MOVEMENT_TYPE_DEBUG: physics_integrate(world, actor); break;
    default: break;
    }
}

static void update_actors(World *world, Population *population)
{   
    for (u32 actor_index = 0; actor_index < ACTOR_MAX; ++actor_index)
    {
        if (population->actor_pool.generation_array[actor_index] == 0)
        {
            continue;
        }
        
        Actor *actor = &population->actor_pool.actor_array[actor_index];

        update_actor(world, actor);
    }
}


void population_get_actor_forward(Actor *actor, vec3 out_forward)
{
    const f32 rotation_x = glm_rad(actor->rotation[0]);
    const f32 rotation_z = glm_rad(actor->rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void population_get_actor_right(Actor *actor, vec3 out_right)
{
    vec3 forward;
    population_get_actor_forward(actor, forward);

    glm_vec3_cross(forward, GLM_ZUP, out_right);

    glm_vec3_normalize(out_right);
}

void population_get_actor_up(Actor *actor, vec3 out_up)
{
    vec3 forward;
    population_get_actor_forward(actor, forward);

    vec3 right;
    population_get_actor_right(actor, right);

    glm_vec3_cross(forward, right, out_up);
    
    glm_vec3_normalize(out_up);
}

i32 population_nation_type_index_from_string(const char *nation_type_string)
{
    for (u32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
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
    for (u32 actor_type_index = 0; actor_type_index < ACTOR_TYPE_COUNT; ++actor_type_index)
    {
        if (strcmp(actor_type_string, ACTOR_TYPE_STRING[actor_type_index]) == 0)
        {
            return actor_type_index;
        }
    }

    return -1;
}

void population_init(Population *population)
{
    init_actor_pool(population);

    init_nations(population);
    
    init_judge(population);
    init_agents(population);
}

void population_update(World *world, Population *population)
{
    update_actors(world, population);
}

void population_close()
{

}
