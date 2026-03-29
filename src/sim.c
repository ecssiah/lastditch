#include "sim.h"

#include "jsk_log.h"
#include "ld_data.h"
#include "action.h"
#include "actor.h"

static void actors_init(Sim *sim)
{
    sim->judge_handle.index = 0;
    sim->judge_handle.generation = 0;

    Actor judge_actor;
    judge_actor.actor_type = ACTOR_TYPE_JUDGE;
    
    judge_actor.world_position[0] = WORLD_CENTER;
    judge_actor.world_position[1] = WORLD_CENTER;
    judge_actor.world_position[2] = TOWER_ROOF_HEIGHT + 5.0f;

    judge_actor.rotation[0] = 0.0f;
    judge_actor.rotation[1] = 0.0f;
    judge_actor.rotation[2] = 90.0f;

    judge_actor.speed = JUDGE_DEFAULT_SPEED;

    judge_actor.velocity[0] = 0.0f;
    judge_actor.velocity[1] = 0.0f;
    judge_actor.velocity[2] = 0.0f;

    sim->actor_pool.actor_array[sim->judge_handle.index] = judge_actor;
}

static void apply_action(Sim *sim, Action *action)
{
    Actor *actor = &sim->actor_pool.actor_array[action->handle.index];
    
    switch (action->type)
    {
    case ACTION_MOVE:
    {
        vec3 velocity_forward;
        vec3 velocity_right;
        vec3 velocity_up;

        vec3 actor_forward;
        actor_get_forward(actor, actor_forward);
        
        vec3 actor_right;
        actor_get_right(actor, actor_right);
        
        glm_vec3_scale(actor_right, action->action_value[0], velocity_right);
        glm_vec3_scale(actor_forward, action->action_value[1], velocity_forward);
        glm_vec3_scale(GLM_ZUP, action->action_value[2], velocity_up);
    
        glm_vec3_zero(actor->velocity);

        glm_vec3_add(actor->velocity, velocity_right, actor->velocity);
        glm_vec3_add(actor->velocity, velocity_forward, actor->velocity);
        glm_vec3_add(actor->velocity, velocity_up, actor->velocity);

        glm_vec3_scale(actor->velocity, actor->speed * sim->delta_time, actor->velocity);

        glm_vec3_add(actor->world_position, actor->velocity, actor->world_position);

        break;
    }
    case ACTION_ROTATE:
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

        break;
    }
    case ACTION_JUMP:
    {
        break;
    }
    default:
    {
        break;
    }
    }

}

void sim_init(Sim *sim)
{
    sim->active = TRUE;
    
    sim->seed = 813;
    // u32 seed = (u32)time(NULL);
    
    srand(sim->seed);

    sim->current_time = 0.0;
    sim->previous_time = 0.0;

    sim->delta_time = 0.0f;
    
    sim->cell_array = calloc(WORLD_VOLUME_IN_CELLS, sizeof(Cell));

    actors_init(sim);
}

void sim_update(Sim *sim)
{
    sim->current_time = glfwGetTime();

    sim->delta_time = (sim->previous_time > 0.0)
        ? (f32)(sim->current_time - sim->previous_time)
        : 0.0f;

    sim->previous_time = sim->current_time;
    
    while (sim->action_queue.head_index != sim->action_queue.tail_index)
    {
        Action action;
        action_pop(&sim->action_queue, &action);

        apply_action(sim, &action);
    }
}

void sim_close(Sim *sim)
{
    free(sim->cell_array);
}
