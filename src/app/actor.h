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

inline const char* ACTOR_TYPE_STRING_ARRAY[]
{
    "None",
    "Judge",
    "Agent",
};

class Actor
{
public:
    static s32 find_type_index(const std::string& actor_type_string);

    s32 id { -1 };

    ActorType actor_type { ActorType::None };
    NationType nation_type { NationType::Wolf };

    Vec3 position {};
    Vec3 rotation {};

    f32 move_speed { ACTOR_DEFAULT_MOVE_SPEED };

    Vec3 velocity {};

    Vec3 target {};

    b32 is_grounded { false };

    BoxCollider box_collider {};
};
