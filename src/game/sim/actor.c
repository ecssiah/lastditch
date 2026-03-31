#include "game/sim/actor.h"

void actor_get_forward(Actor *actor, vec3 out_forward)
{
    const f32 rotation_x = glm_rad(actor->rotation[0]);
    const f32 rotation_z = glm_rad(actor->rotation[2]);

    out_forward[0] = cosf(rotation_x) * cosf(rotation_z);
    out_forward[1] = cosf(rotation_x) * sinf(rotation_z);
    out_forward[2] = sinf(rotation_x);

    glm_vec3_normalize(out_forward);
}

void actor_get_right(Actor *actor, vec3 out_right)
{
    vec3 forward;
    actor_get_forward(actor, forward);

    glm_vec3_cross(forward, GLM_ZUP, out_right);

    glm_vec3_normalize(out_right);
}

void actor_get_up(Actor *actor, vec3 out_up)
{
    vec3 forward;
    actor_get_forward(actor, forward);

    vec3 right;
    actor_get_right(actor, right);

    glm_vec3_cross(forward, right, out_up);
    
    glm_vec3_normalize(out_up);
}

ActorHandle actor_add_to_pool(Actor *actor, ActorPool* actor_pool)
{   
    const u32 actor_index = actor_pool->free_array[--actor_pool->free_count];
    const u32 actor_generation = ++actor_pool->generation_array[actor_index];
    
    actor_pool->actor_array[actor_index] = *actor;
    
    actor_pool->active_array[actor_index] = actor_pool->active_count;
    actor_pool->active_array[actor_pool->active_count++] = actor_index;

    assert(actor_pool->free_count + actor_pool->active_count == MAX_ACTORS);

    ActorHandle actor_handle = { actor_index, actor_generation };
    
    return actor_handle;
}
