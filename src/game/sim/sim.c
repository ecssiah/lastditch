#include "game/sim/sim.h"

#include <string.h>
#include <time.h>

#include "game/sim/population.h"
#include "jsk_log.h"

#include "core/action.h"
#include "game/sim/actor.h"
#include "game/sim/physics.h"
#include "game/sim/sim_data.h"
#include "game/sim/world.h"

static void time_init(Sim *sim)
{
    sim->world.delta_time = 0.0f;

    sim->world.second_count = 0;
    sim->world.tick_count = 0;

    sim->world.time_rate = 1.0f;
}

static void physics_init(Sim *sim)
{
    sim->world.gravity[0] = 0.0f;
    sim->world.gravity[1] = 0.0f;
    sim->world.gravity[2] = GRAVITY_DEFAULT;
}

static void apply_move_action(Actor *actor, Action *action)
{
    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;

    vec3 actor_forward;
    actor_get_forward(actor, actor_forward);
        
    vec3 actor_right;
    actor_get_right(actor, actor_right);

    switch (actor->movement_type)
    {
    case MOVEMENT_TYPE_GROUND:
    {
        vec3 actor_forward_flat;
        actor_forward_flat[0] = actor_forward[0];
        actor_forward_flat[1] = actor_forward[1];
        actor_forward_flat[2] = 0.0f;

        glm_vec3_scale(actor_right, action->action_value[0], velocity_right);
        glm_vec3_scale(actor_forward_flat, action->action_value[1], velocity_forward);

        vec3 move_velocity;
        memset(move_velocity, 0, sizeof(move_velocity));

        glm_vec3_add(move_velocity, velocity_right, move_velocity);
        glm_vec3_add(move_velocity, velocity_forward, move_velocity);

        glm_vec3_scale(move_velocity, actor->speed, move_velocity);

        if (glm_vec3_norm(move_velocity) > 0.0f)
        {
            glm_vec3_normalize(move_velocity);
            glm_vec3_scale(move_velocity, actor->speed, move_velocity);
        }

        actor->velocity[0] = move_velocity[0];
        actor->velocity[1] = move_velocity[1];
        
        break;
    }
    case MOVEMENT_TYPE_DEBUG:
    {
        glm_vec3_scale(actor_right, action->action_value[0], velocity_right);
        glm_vec3_scale(actor_forward, action->action_value[1], velocity_forward);
        glm_vec3_scale(GLM_ZUP, action->action_value[2], velocity_up);

        actor->velocity[0] = 0.0f;
        actor->velocity[1] = 0.0f;
        actor->velocity[2] = 0.0f;
    
        glm_vec3_add(actor->velocity, velocity_right, actor->velocity);
        glm_vec3_add(actor->velocity, velocity_forward, actor->velocity);
        glm_vec3_add(actor->velocity, velocity_up, actor->velocity);

        glm_vec3_scale(actor->velocity, actor->speed, actor->velocity);
        
        break;
    }
    default: break;
    }
}

static void apply_rotate_action(Actor *actor, Action *action)
{
    actor->rotation[2] -= CAMERA_SENSITIVITY_X * action->action_value[0];
    actor->rotation[0] -= CAMERA_SENSITIVITY_Y * action->action_value[1];
    
    if (actor->rotation[0] > CAMERA_PITCH_LIMIT)
    {
        actor->rotation[0] = CAMERA_PITCH_LIMIT;
    }
    
    if (actor->rotation[0] < -CAMERA_PITCH_LIMIT)
    {
        actor->rotation[0] = -CAMERA_PITCH_LIMIT;
    }
}

static void apply_jump_action(Actor *actor)
{
    if (actor->is_grounded)
    {
        actor->velocity[2] = JUDGE_DEFAULT_JUMP_SPEED;
    }
}

static void apply_debug_mode_action(Actor *actor)
{
    switch (actor->movement_type)
    {
    case MOVEMENT_TYPE_GROUND:
    {
        actor->movement_type = MOVEMENT_TYPE_DEBUG;
        actor->speed = JUDGE_DEFAULT_DEBUG_SPEED;

        actor->box_collider.collision_enabled = false;

        break;
    }
    case MOVEMENT_TYPE_DEBUG:
    {
        actor->movement_type = MOVEMENT_TYPE_GROUND;
        actor->speed = JUDGE_DEFAULT_GROUND_SPEED;

        actor->box_collider.collision_enabled = true;
        
        break;
    }
    default: break;
    }
}

static void apply_action(Sim *sim, Action *action)
{
    Actor *actor = &sim->population.actor_pool.actor_array[sim->population.judge_handle.index];

    switch (action->type)
    {
    case ACTION_MOVE: apply_move_action(actor, action); break;
    case ACTION_ROTATE: apply_rotate_action(actor, action); break;
    case ACTION_JUMP: apply_jump_action(actor); break;
    case ACTION_DEBUG_MODE: apply_debug_mode_action(actor); break;
    default: break;
    }
}

static void apply_action_queue(Sim *sim)
{
    while (sim->action_queue.head_index != sim->action_queue.tail_index)
    {
        Action action;
        action_pop(&sim->action_queue, &action);

        apply_action(sim, &action);
    }
}

static void update_actor(Sim *sim, Actor *actor)
{
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

void sim_init(Sim *sim)
{
    sim->active = true;
    sim->evolving = true;

    const bool random_seed = false;
    
    sim->seed = random_seed ? (u32)time(NULL) : 813;
    
    srand(sim->seed);

    time_init(sim);
    population_init(sim);
    world_init(sim);
    physics_init(sim);
}

void sim_update(Sim *sim)
{
    apply_action_queue(sim);

    update_actors(sim);
}

void sim_close(Sim *sim)
{
    population_close(sim);
    world_close(sim);
}
