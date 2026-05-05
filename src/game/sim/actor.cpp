#include "game/sim/actor.h"

#include <string.h>

#include "core/math_ext.h"
#include "game/sim/world.h"

const char *ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE(DEFINE_LIST_STRING)
};

i32 actor_type_index_from_string(const char *actor_type_string)
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

vec3 actor_get_forward(Actor *actor)
{
    const f32 rotation_x = glm::radians(actor->rotation[0]);
    const f32 rotation_z = glm::radians(actor->rotation[2]);

    vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };
    
    return glm::normalize(forward);
}

vec3 actor_get_right(Actor *actor)
{
    vec3 forward = actor_get_forward(actor);

    vec3 right = glm::cross(forward, { 0, 0, 1 });

    return glm::normalize(right);
}

vec3 actor_get_up(Actor *actor)
{
    vec3 forward = actor_get_forward(actor);
    vec3 right = actor_get_right(actor);

    vec3 up = glm::cross(forward, right);

    return glm::normalize(up);
}

void actor_add(ActorPool *actor_pool, Actor *actor)
{
    const ActorID actor_id = actor_pool->free_array[--actor_pool->free_count];

    actor->actor_id = actor_id;
    
    actor_pool->active_array[actor_pool->active_count++] = actor_id;
    actor_pool->actor_array[actor_id] = *actor;
    
    assert(actor_pool->free_count + actor_pool->active_count == ACTOR_MAX);
}

// void actor_control(World *world, Actor *actor)
// {
//     switch (actor->actor_control.control_type)
//     {
//     case CONTROL_TYPE_WANDER:
//     {
//         if (actor->actor_control.decision_clock < actor->actor_control.decision_period)
//         {
//             actor->actor_control.decision_clock++;
//         }
//         else
//         {
//             const i32 direction_angle = rand() % 360;

//             const vec2 direction = {
//                 cosf(glm_rad(direction_angle)),
//                 sinf(glm_rad(direction_angle))
//             };

//             actor->velocity[0] = direction[0] * AGENT_DEFAULT_GROUND_SPEED;
//             actor->velocity[1] = direction[1] * AGENT_DEFAULT_GROUND_SPEED;

//             actor->rotation_target[2] = direction_angle;

//             actor->actor_control.decision_clock = 0;
//         }

//         actor->rotation[2] = lerp_to(
//             actor->rotation[2],
//             actor->rotation_target[2],
//             5.0f,
//             world->delta_time
//         );

//         break;
//     }
//     case CONTROL_TYPE_SEEK:
//     {
//         break;
//     }
//     case CONTROL_TYPE_NONE: break;
//     default: break;
//     }
// }



