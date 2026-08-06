#pragma once

#include "nation.h"
#include "physics.h"
#include "core/geometry.h"
#include "core/types.h"

constexpr f32 ACTOR_DEFAULT_MOVE_SPEED { 12.0f };
constexpr f32 ACTOR_DEFAULT_JUMP_SPEED { 28.0f };

enum class ActorType : u8
{
    None,
    Judge,
    Agent,
    COUNT,
};

constexpr s32 ACTOR_TYPE_COUNT { static_cast<s32>(ActorType::COUNT) };

class Actor
{
public:
    s32 id { -1 };

    ActorType actor_type { ActorType::None };
    NationType nation_type { NationType::Wolf };

    f32 move_speed { ACTOR_DEFAULT_MOVE_SPEED };

    Vec3 position {};
    Vec3 rotation {};
    Vec3 velocity {};

    Vec3 position_target {};
    Vec3 rotation_target {};

    b32 is_grounded { false };

    BoxCollider box_collider {};
};
