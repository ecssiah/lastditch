#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr f32 MAX_VELOCITY = 200.0f;

constexpr f32 GRAVITY_CONSTANT = -90.0f;

constexpr f32 RISING_GRAVITY_MODIFIER = 1.0f;
constexpr f32 FALLING_GRAVITY_MODIFIER = 1.7f;

struct Actor;
class World;

struct BoxCollider
{
    b32 collision_enabled;

    Vec3 radius;
};

void physics_update_actor(f32 delta_time, Actor& actor, World& world);
