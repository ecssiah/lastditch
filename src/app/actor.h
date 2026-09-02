#pragma once

#include "constants.h"
#include "nation.h"
#include "physics.h"
#include "core/geometry.h"
#include "core/types.h"

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
    NationType nation_type { NationType::wolf };

    f32 move_speed { ACTOR_DEFAULT_MOVE_SPEED };
    f32 turn_speed { ACTOR_DEFAULT_TURN_SPEED };

    Vec3 position {};
    Vec3 rotation {};
    Vec3 velocity {};

    Vec3 position_target {};
    Vec3 rotation_target {};

    BoxCollider box_collider {};

    b32 is_grounded { false };

    s32 decision_timer { 0 };
};
