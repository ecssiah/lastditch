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
