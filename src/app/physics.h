#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr f32 MAX_VELOCITY = 200.0f;

constexpr f32 GRAVITY_CONSTANT = -90.0f;

constexpr f32 RISING_GRAVITY_MODIFIER = 1.0f;
constexpr f32 FALLING_GRAVITY_MODIFIER = 1.7f;

class Actor;
class World;

struct BoxCollider
{
    b32 collision_enabled;

    Vec3 radius;
};

class Physics
{
public:
    Physics();

    static void update_actor(World& world, Actor& actor, f32 delta_time);

private:
    static Bounds3 get_box_collider_bounds(const BoxCollider& box_collider, const Vec3& position);
    static IBounds3 get_grid_overlap_of_bounds(const Bounds3& bounds);
    static void resolve_axis_collisions(World& world, Actor& actor, Axis axis, f32 step_delta_time);
    static void integrate(World& world, Actor& actor, f32 delta_time);

    Vec3 gravity;
};

