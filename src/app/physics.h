#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr f32 max_velocity = 200.0f;

constexpr f32 rising_gravity_modifier = 1.0f;
constexpr f32 falling_gravity_modifier = 1.7f;

struct Actor;
struct World;

struct BoxCollider
{
    b32 collision_enabled;

    Vec3 radius;
};

void physics_update_actor(Actor& actor, World& world);
