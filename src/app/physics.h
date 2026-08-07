#pragma once

#include "constants.h"
#include "core/geometry.h"
#include "core/types.h"

class Actor;
class World;

class BoxCollider
{
public:
    b32 collision_enabled { true };

    Vec3 radius {};
};

class Physics
{
public:
    Physics() = default;

    static void update_actor(World& world, Actor& actor);

    f32 gravity { GRAVITY_CONSTANT };

private:
    static Bounds3 get_box_collider_bounds(const BoxCollider& box_collider, const Vec3& position);
    static IBounds3 get_grid_overlap_of_bounds(const Bounds3& bounds);
    static void resolve_axis_collisions(World& world, Actor& actor, Axis axis, f32 step_delta_time);
};

