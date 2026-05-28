#pragma once

#include "app/nation.h"
#include "app/physics.h"
#include "core/geometry.h"
#include "core/types.h"

constexpr s32 actor_max = 256;

constexpr f32 agent_default_ground_speed = 1.0f;
constexpr f32 agent_default_jump_speed = 28.0f;

constexpr f32 judge_default_ground_speed = 12.0f;
constexpr f32 judge_default_jump_speed = 36.0f;
constexpr f32 judge_default_debug_speed = 32.0f;

constexpr f32 camera_sensitivity_x = 0.22f;
constexpr f32 camera_sensitivity_y = 0.22f;
constexpr f32 camera_pitch_limit = 89.0f;

constexpr s32 actor_task_max_count = 128;

#define FOR_LIST_MOVEMENT_TYPE(DO)                                             \
    DO(Ground)                                                                 \
    DO(Debug)

enum class MovementType : u8
{
    FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 MOVEMENT_TYPE_COUNT = FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_COUNT);

#define FOR_LIST_ACTOR_TYPE(DO)                                                \
    DO(None)                                                                     \
    DO(Judge)                                                                    \
    DO(Agent)

enum ActorType
{
    FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 actor_type_count = FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_COUNT);

extern const char* actor_type_string_array[actor_type_count];

struct Actor
{
    s32 actor_id;

    ActorType actor_type;
    NationType nation_type;
    MovementType movement_type;

    Vec3 position;

    Vec3 rotation;
    Vec3 rotation_target;

    b32 is_grounded;

    f32 speed;
    Vec3 velocity;

    BoxCollider box_collider;

    s32 act_id_count;
    std::array<s32, actor_task_max_count> act_id_array;
};

struct ActorPool
{
    s32 active_count;
    std::array<s32, actor_max> active_array;
    std::array<s32, actor_max> active_lookup;

    s32 free_count;
    std::array<s32, actor_max> free_array;

    std::array<Actor, actor_max> actor_array;
};

s32 actor_type_index_from_string(const std::string& actor_type_string);

void actor_add(ActorPool& actor_pool, Actor& actor);
void actor_control(World& world, Actor& actor);
