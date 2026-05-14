#include "game/sim/actor.h"

#include <cassert>

#include "game/sim/world.h"

const char* ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_STRINGS)
};

i32 
actor_type_index_from_string(const std::string& actor_type_string)
{
    for (i32 actor_type_index = 0; actor_type_index < ACTOR_TYPE_COUNT; ++actor_type_index)
    {
        if (actor_type_string == ACTOR_TYPE_STRING[actor_type_index])
        {
            return actor_type_index;
        }
    }

    return -1;
}

void 
actor_add(ActorPool& actor_pool, Actor& actor)
{
    const ActorID actor_id = actor_pool.free_array[--actor_pool.free_count];

    actor.actor_id = actor_id;

    actor_pool.active_array[actor_pool.active_count++] = actor_id;
    actor_pool.actor_array[actor_id] = actor;

    assert(actor_pool.free_count + actor_pool.active_count == ACTOR_MAX);
}

// void 
// actor_control(World *world, Actor *actor)
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

//         actor->rotation[2] = interpolate_to(
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
