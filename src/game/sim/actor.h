#ifndef ACTOR_H
#define ACTOR_H

#include <cglm/cglm.h>

#include "jsk.h"

#include "game/sim/nation.h"
#include "game/sim/physics.h"

#define ACTOR_MAX 256u

#define AGENT_DEFAULT_GROUND_SPEED 1.0f
#define AGENT_DEFAULT_JUMP_SPEED 28.0f

#define JUDGE_DEFAULT_GROUND_SPEED 12.0f
#define JUDGE_DEFAULT_JUMP_SPEED 46.0f
#define JUDGE_DEFAULT_DEBUG_SPEED 32.0f

#define CAMERA_SENSITIVITY_X 0.1f
#define CAMERA_SENSITIVITY_Y 0.1f

#define CAMERA_PITCH_LIMIT 89.99f

typedef enum MovementType MovementType;
enum MovementType
{
    MOVEMENT_TYPE_GROUND,
    MOVEMENT_TYPE_DEBUG,

    MOVEMENT_TYPE_COUNT,
};

typedef enum ControlType ControlType;
enum ControlType
{
    CONTROL_TYPE_NONE,
    CONTROL_TYPE_WANDER,
    CONTROL_TYPE_SEEK,
};

typedef struct ActorControl ActorControl;
struct ActorControl
{
    ControlType control_type;
    
    u32 decision_clock;
    u32 decision_period;
};

#define FOR_LIST_ACTOR_TYPE(DO)                 \
    DO(ACTOR_TYPE_FREE)                         \
    DO(ACTOR_TYPE_JUDGE)                        \
    DO(ACTOR_TYPE_AGENT)                        \

typedef enum ActorType ActorType;
enum ActorType
{
    FOR_LIST_ACTOR_TYPE( DEFINE_LIST_ENUMERATION )
    ACTOR_TYPE_COUNT,
};

extern const char *ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT];

typedef struct Actor Actor;
struct Actor
{
    ActorType actor_type;
    NationType nation_type;
    MovementType movement_type;
    
    vec3 position;
    
    vec3 rotation;
    vec3 rotation_target;

    b32 is_grounded;
    
    f32 speed;
    vec3 velocity;

    BoxCollider box_collider;

    ActorControl actor_control;
};

typedef struct ActorHandle ActorHandle;
struct ActorHandle
{
    u32 index;
    u32 generation;
};

typedef struct ActorPool ActorPool;
struct ActorPool
{
    Actor actor_array[ACTOR_MAX];

    u32 generation_array[ACTOR_MAX];

    u32 active_array[ACTOR_MAX];
    u32 active_count;
    
    u32 free_array[ACTOR_MAX];
    u32 free_count;
};

i32 actor_type_index_from_string(const char *actor_type_string);

void actor_get_forward(Actor *actor, vec3 out_forward);
void actor_get_right(Actor *actor, vec3 out_right);
void actor_get_up(Actor *actor, vec3 out_up);

ActorHandle actor_add(ActorPool *actor_pool, Actor *actor);
void actor_control(World *world, Actor *actor);

void actor_init_pool(ActorPool *actor_pool);

#endif
