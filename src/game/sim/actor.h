#pragma once

#include "core/geometry.h"
#include "core/types.h"
#include "game/sim/ids.h"
#include "game/sim/nation.h"
#include "game/sim/physics.h"

constexpr i32 ACTOR_MAX = 256;

constexpr f32 AGENT_DEFAULT_GROUND_SPEED = 1.0f;
constexpr f32 AGENT_DEFAULT_JUMP_SPEED = 28.0f;

constexpr f32 JUDGE_DEFAULT_GROUND_SPEED = 12.0f;
constexpr f32 JUDGE_DEFAULT_JUMP_SPEED = 28.0f;
constexpr f32 JUDGE_DEFAULT_DEBUG_SPEED = 32.0f;

constexpr f32 CAMERA_SENSITIVITY_X = 0.2f;
constexpr f32 CAMERA_SENSITIVITY_Y = 0.2f;
constexpr f32 CAMERA_PITCH_LIMIT = 89.99f;

constexpr i32 ACTOR_ACT_MAX = 128;

#define FOR_LIST_MOVEMENT_TYPE(DO)                                             \
    DO(ground)                                                                 \
    DO(debug)

enum class MovementType
{
    FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 MOVEMENT_TYPE_COUNT = FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_COUNT);

#define FOR_LIST_ACTOR_TYPE(DO)                                                \
  DO(none)                                                                     \
  DO(judge)                                                                    \
  DO(agent)

enum ActorType
{
    FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 ACTOR_TYPE_COUNT = FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_COUNT);

extern const char* ACTOR_TYPE_STRING[ACTOR_TYPE_COUNT];

struct Actor
{
    ActorID actor_id;

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

    i32 act_id_count;
    ActID act_id_array[ACTOR_ACT_MAX];
};

struct ActorPool
{
    i32 active_count;
    ActorID active_array[ACTOR_MAX];
    i32 active_lookup[ACTOR_MAX];

    i32 free_count;
    ActorID free_array[ACTOR_MAX];

    Actor actor_array[ACTOR_MAX];
};

i32 actor_type_index_from_string(const char* actor_type_string);

void actor_add(ActorPool& actor_pool, Actor& actor);
void actor_control(World& world, Actor& actor);
