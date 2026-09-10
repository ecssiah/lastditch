#pragma once

#include "collider.h"
#include "constants.h"
#include "nation.h"
#include "core/geometry.h"
#include "core/types.h"
#include "core/geometry/quaternion.h"

enum class ActorType : u8
{
    None,
    Judge,
    Agent,
    COUNT,
};

constexpr s32 ACTOR_TYPE_COUNT { static_cast<s32>(ActorType::COUNT) };

struct Actor
{
    s32 id { -1 };

    ActorType actor_type { ActorType::None };
    NationType nation_type { NationType::wolf };

    f32 move_speed { ACTOR_DEFAULT_MOVE_SPEED };
    f32 turn_speed { ACTOR_DEFAULT_TURN_SPEED };

    Vec3 position {};
    Vec3 position_target {};

    Quaternion orientation {};
    Vec3 rotation_target {};
    f32 movement_yaw {};

    Vec3 velocity {};

    Collider box_collider {};

    b32 is_grounded { false };

    s32 decision_timer { 0 };
};
