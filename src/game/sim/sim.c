#include "game/sim/sim.h"

#include <string.h>
#include <time.h>

#include "game/sim/debug.h"
#include "game/sim/population.h"
#include "game/sim/world.h"

static void apply_move_action(Actor *actor, Action *action)
{
    vec3 velocity_forward;
    vec3 velocity_right;
    vec3 velocity_up;

    vec3 actor_forward;
    population_get_actor_forward(actor, actor_forward);
        
    vec3 actor_right;
    population_get_actor_right(actor, actor_right);

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

        vec3 move_velocity = GLM_VEC3_ZERO_INIT;

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
    case ACTION_MOVE:         apply_move_action(actor, action); break;
    case ACTION_ROTATE:       apply_rotate_action(actor, action); break;
    case ACTION_JUMP:         apply_jump_action(actor); break;
    case ACTION_DEBUG_MODE:   apply_debug_mode_action(actor); break;
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

void sim_init(Sim *sim)
{
    sim->active = true;
    sim->evolving = true;

    const b32 random_seed = false;
    
    sim->seed = random_seed ? (u32)time(NULL) : 813;
    
    srand(sim->seed);

    debug_init(&sim->debug);
    population_init(&sim->population);
    world_init(&sim->world);

    vec3 position_a = { 1.0f, 1.0f, 1.0f };
    vec3 position_b = { 6.0f, 1.0f, 1.0f };
    vec3 color = { 1.0f, 0.0f, 0.0f };
    
    debug_draw_line(&sim->debug, position_a, position_b, color);
}

void sim_update(Sim *sim)
{
    apply_action_queue(sim);

    population_update(&sim->world, &sim->population);
}

void sim_close(Sim *sim)
{
    debug_close(&sim->debug);
    population_close();
    world_close(&sim->world);
}
