#pragma once

#include "pool.h"
#include "app/nation.h"
#include "app/physics.h"
#include "core/geometry.h"
#include "core/types.h"

constexpr s32 ACTOR_POOL_MAX {1 << 8};

constexpr f32 AGENT_DEFAULT_GROUND_SPEED {1.0f};
constexpr f32 AGENT_DEFAULT_JUMP_SPEED {28.0f};

constexpr f32 JUDGE_DEFAULT_GROUND_SPEED {12.0f};
constexpr f32 JUDGE_DEFAULT_JUMP_SPEED {36.0f};
constexpr f32 JUDGE_DEFAULT_DEBUG_SPEED {32.0f};

constexpr f32 CAMERA_SENSITIVITY_X {0.22f};
constexpr f32 CAMERA_SENSITIVITY_Y {0.22f};
constexpr f32 CAMERA_PITCH_LIMIT {89.0f};

constexpr s32 ACTOR_TASK_MAX_COUNT {128};

#define FOR_LIST_MOVEMENT_TYPE(DO)                                             \
    DO(Ground)                                                                 \
    DO(Debug)

enum class MovementType : u8
{
    FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 MOVEMENT_TYPE_COUNT = FOR_LIST_MOVEMENT_TYPE(DEFINE_ENUM_COUNT);

#define FOR_LIST_ACTOR_TYPE(DO)                                                 \
    DO(None)                                                                    \
    DO(Judge)                                                                   \
    DO(Agent)

enum ActorType
{
    FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr s32 ACTOR_TYPE_COUNT = FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_COUNT);

extern const char* actor_type_string_array[ACTOR_TYPE_COUNT];

class Actor
{
public:
    s32 id {};

    ActorType actor_type {ActorType::None};
    NationType nation_type {NationType::Wolf};
    MovementType movement_type {MovementType::Ground};

    Vec3 position {};
    Vec3 rotation {};
    Vec3 rotation_target {};

    b32 is_grounded {false};

    f32 speed {0.0f};
    Vec3 velocity {};

    BoxCollider box_collider {};

    s32 act_id_count {};
    std::array<s32, ACTOR_TASK_MAX_COUNT> act_id_array {};

    static s32 get_type_index(const std::string& actor_type_string);
};

using ActorPool = Pool<Actor, ACTOR_POOL_MAX>;
