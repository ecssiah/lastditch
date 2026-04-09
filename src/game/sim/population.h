#ifndef POPULATION_H
#define POPULATION_H

#include <cglm/cglm.h>

#include "jsk.h"

#include "game/sim/world.h"

#define ACTOR_MAX 256u

#define AGENT_INITIAL_POPULATION 12u

#define AGENT_DEFAULT_GROUND_SPEED 1.0f
#define AGENT_DEFAULT_JUMP_SPEED 28.0f

#define RISING_GRAVITY_MODIFIER 1.0f
#define FALLING_GRAVITY_MODIFIER 1.7f

#define JUDGE_DEFAULT_GROUND_SPEED 12.0f
#define JUDGE_DEFAULT_JUMP_SPEED 46.0f
#define JUDGE_DEFAULT_DEBUG_SPEED 32.0f

#define CAMERA_SENSITIVITY_X 0.1f
#define CAMERA_SENSITIVITY_Y 0.1f

#define CAMERA_PITCH_LIMIT 89.99f

typedef struct BoxCollider BoxCollider;
struct BoxCollider
{
    b32 collision_enabled;
    
    vec3 radius;
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

typedef enum MovementType MovementType;
enum MovementType
{
    MOVEMENT_TYPE_GROUND,
    MOVEMENT_TYPE_DEBUG,

    MOVEMENT_TYPE_COUNT,
};

typedef struct ActorHandle ActorHandle;
struct ActorHandle
{
    u32 index;
    u32 generation;
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

#define FOR_LIST_NATION_TYPE(DO)                \
    DO(NATION_TYPE_WOLF)                        \
    DO(NATION_TYPE_EAGLE)                       \
    DO(NATION_TYPE_LION)                        \
    DO(NATION_TYPE_BEAR)                       \

typedef enum NationType NationType;
enum NationType
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_ENUMERATION )
    NATION_TYPE_COUNT
};

extern const char *NATION_TYPE_STRING[NATION_TYPE_COUNT];

typedef struct Nation Nation;
struct Nation
{
    NationType nation_type;

    ivec3 home_coordinate;
};

typedef struct Actor Actor;
struct Actor
{
    ActorType actor_type;
    MovementType movement_type;
    NationType nation_type;
    
    vec3 position;
    
    vec3 rotation;
    vec3 rotation_target;

    b32 is_grounded;
    
    f32 speed;
    vec3 velocity;

    BoxCollider box_collider;

    ActorControl actor_control;
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

typedef struct Population Population;
struct Population
{
    ActorHandle judge_handle;
    
    ActorPool actor_pool;

    Nation nation_array[NATION_TYPE_COUNT];
};

void population_get_actor_forward(Actor *actor, vec3 out_forward);
void population_get_actor_right(Actor *actor, vec3 out_right);
void population_get_actor_up(Actor *actor, vec3 out_up);

u32 population_actor_type_index_from_string(const char *actor_type_string);
u32 population_nation_type_index_from_string(const char *nation_type_string);
    
void population_init(Population *population);
void population_update(World *world, Population *population);
void population_close();

#endif
