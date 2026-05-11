#pragma once

#include <cglm/cglm.h>

#include "core/types.h"

constexpr f32 RISING_GRAVITY_MODIFIER = 1.0f;
constexpr f32 FALLING_GRAVITY_MODIFIER = 1.7f;

struct Actor;
struct World;

struct BoxCollider
{
    b32 collision_enabled;

    vec3 radius;
};

void physics_update_actor(Actor* actor, World* world);
