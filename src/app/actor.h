#pragma once

#include "nation.h"
#include "physics.h"
#include "core/geometry.h"
#include "core/types.h"

constexpr s32 ACTOR_POOL_MAX { 1 << 8 };

constexpr f32 AGENT_DEFAULT_GROUND_SPEED { 1.0f };
constexpr f32 AGENT_DEFAULT_JUMP_SPEED { 28.0f };

constexpr f32 JUDGE_DEFAULT_GROUND_SPEED { 12.0f };
constexpr f32 JUDGE_DEFAULT_JUMP_SPEED { 36.0f };
constexpr f32 JUDGE_DEFAULT_DEBUG_SPEED { 32.0f };

constexpr f32 CAMERA_SENSITIVITY_X { 0.22f };
constexpr f32 CAMERA_SENSITIVITY_Y { 0.22f };
constexpr f32 CAMERA_PITCH_LIMIT { 89.0f };

constexpr s32 ACTOR_TASK_MAX_COUNT { 128 };

enum class MovementType : u8
{
    Ground,
    Air,
    COUNT,
};

enum class ActorType : u8
{
    None,
    Judge,
    Agent,
    COUNT,
};

constexpr s32 ACTOR_TYPE_COUNT { static_cast<s32>(ActorType::COUNT) };

inline const char* ACTOR_TYPE_STRING_ARRAY[]
{
    "None",
    "Judge",
    "Agent",
};

class Actor
{
public:
    static s32 get_type_index(const std::string& actor_type_string);

    s32 id { -1 };

    ActorType actor_type { ActorType::None };
    NationType nation_type { NationType::Wolf };
    MovementType movement_type { MovementType::Ground };

    Vec3 position {};
    Vec3 rotation {};

    Vec3 position_target {};
    Vec3 rotation_target {};

    f32 speed { 0.0f };
    Vec3 velocity {};

    b32 is_grounded { false };

    BoxCollider box_collider {};
};
