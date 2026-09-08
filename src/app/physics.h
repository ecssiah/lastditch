#pragma once

#include "core/geometry.h"
#include "core/types.h"

struct Collider;
struct Actor;
class World;

class Physics
{
public:
    Physics() = default;

    static void update_actor(World& world, Actor& actor);

private:
    static Bounds3 get_collider_bounds(const Collider& collider, const Vec3& position);
    static IBounds3 get_grid_overlap_of_bounds(const Bounds3& bounds);

    static void resolve_axis_collisions(World& world, Actor& actor, Axis axis, f32 step_delta_time);
};

