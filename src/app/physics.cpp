#include "app/physics.h"

#include <algorithm>
#include <cmath>

#include "core/types.h"
#include "app/actor.h"
#include "app/world.h"

Physics::Physics()
    :
    gravity{0, 0, GRAVITY_CONSTANT}
{

}

void
Physics::update_actor(World& world, Actor& actor, const f32 delta_time)
{
    switch (actor.movement_type)
    {
        case MovementType::Ground:
        case MovementType::Debug:
            integrate(world, actor, delta_time);
            break;
    }
}

Bounds3
Physics::get_box_collider_bounds(const BoxCollider& box_collider, const Vec3& position)
{
    constexpr f32 world_size {WORLD_SIZE_IN_CELLS};
    
    return {
        max(position - box_collider.radius, Vec3{0.0f}),
        min(position + box_collider.radius, Vec3{world_size}),
    };
}

IBounds3
Physics::get_grid_overlap_of_bounds(const Bounds3& bounds)
{
    return {
        {
            max(static_cast<s32>(floorf(bounds.min.x)), 0),
            max(static_cast<s32>(floorf(bounds.min.y)), 0),
            max(static_cast<s32>(floorf(bounds.min.z)), 0),
        },
        {
            min(static_cast<s32>(ceilf(bounds.max.x)) - 1, WORLD_SIZE_IN_CELLS - 1),
            min(static_cast<s32>(ceilf(bounds.max.y)) - 1, WORLD_SIZE_IN_CELLS - 1),
            min(static_cast<s32>(ceilf(bounds.max.z)) - 1, WORLD_SIZE_IN_CELLS - 1),
        },
    };
}

void
Physics::resolve_axis_collisions(World& world, Actor& actor, Axis axis, const f32 step_delta_time)
{
    if (actor.velocity[static_cast<s32>(axis)] == 0.0f)
    {
        return;
    }

    Bounds3 actor_bounds {get_box_collider_bounds(actor.box_collider, actor.position)};
    Bounds3 swept_bounds {};
    
    for (s32 axis_index = 0; axis_index < AXIS_COUNT; ++axis_index)
    {
        swept_bounds.min[axis_index] = min(
            actor_bounds.min[axis_index],
            actor_bounds.min[axis_index] + step_delta_time * actor.velocity[axis_index]
        );

        swept_bounds.max[axis_index] = max(
            actor_bounds.max[axis_index],
            actor_bounds.max[axis_index] + step_delta_time * actor.velocity[axis_index]
        );
    }

    const IBounds3 grid_overlap_bounds {get_grid_overlap_of_bounds(swept_bounds)};

    const s32 axis_index {static_cast<s32>(axis)};

    const f32 actor_min_prev {actor_bounds.min[axis_index]};
    const f32 actor_max_prev {actor_bounds.max[axis_index]};

    const f32 actor_min_next {actor_min_prev + step_delta_time * actor.velocity[axis_index]};
    const f32 actor_max_next {actor_max_prev + step_delta_time * actor.velocity[axis_index]};

    const s32 axis_s {(axis_index + 1) % AXIS_COUNT};
    const s32 axis_t {(axis_index + 2) % AXIS_COUNT};

    b32 found {false};
    f32 best {actor.velocity[axis_index] > 0 ? INFINITY : -INFINITY};

    constexpr s32 axis_z_index {static_cast<s32>(Axis::Z)};
    constexpr s32 axis_y_index {static_cast<s32>(Axis::Y)};
    constexpr s32 axis_x_index {static_cast<s32>(Axis::X)};
    
    for (s32 z = grid_overlap_bounds.min[axis_z_index]; z <= grid_overlap_bounds.max[axis_z_index]; ++z) 
    {
        for (s32 y = grid_overlap_bounds.min[axis_y_index]; y <= grid_overlap_bounds.max[axis_y_index]; ++y) 
        {
            for (s32 x = grid_overlap_bounds.min[axis_x_index]; x <= grid_overlap_bounds.max[axis_x_index]; ++x) 
            {
                const IVec3 cell_coordinate {x, y, z};

                if (!World::cell_coordinate_is_valid(x, y, z))
                {
                    continue;
                }

                const Cell& cell {world.get_cell(x, y, z)};

                if (cell.block_type == BlockType::None)
                {
                    continue;
                }

                const f32 cell_s {static_cast<f32>(cell_coordinate[axis_s])};

                if (actor_bounds.max[axis_s] <= cell_s || actor_bounds.min[axis_s] >= cell_s + 1.0f)
                {
                    continue;
                }

                const f32 cell_t {static_cast<f32>(cell_coordinate[axis_t])};

                if (actor_bounds.max[axis_t] <= cell_t || actor_bounds.min[axis_t] >= cell_t + 1.0f)
                {
                    continue;
                }

                if (actor.velocity[axis_index] > 0)
                {
                    const f32 block_min {static_cast<f32>(cell_coordinate[axis_index])};

                    if (block_min >= actor_max_prev && block_min <= actor_max_next && block_min < best) 
                    {
                        best = block_min;
                        found = true;
                    }
                }
                else
                {
                    const f32 block_max {static_cast<f32>(cell_coordinate[axis_index]) + 1.0f};

                    if (block_max <= actor_min_prev && block_max >= actor_min_next && block_max > best) 
                    {
                        best = block_max;
                        found = true;
                    }
                }
            }
        }
    }

    if (found)
    {
        if (actor.velocity[axis_index] > 0)
        {
            actor.position[axis_index] = best - actor.box_collider.radius[axis_index];
        }
        else
        {
            actor.position[axis_index] = best + actor.box_collider.radius[axis_index];

            if (axis == Axis::Z)
            {
                actor.is_grounded = true;
            }
        }

        actor.velocity[axis_index] = 0.0f;
    }
    else
    {
        actor.position[axis_index] += step_delta_time * actor.velocity[axis_index];
    }
}

void
Physics::integrate(World& world, Actor& actor, const f32 delta_time)
{
    actor.is_grounded = false;

    if (actor.movement_type == MovementType::Ground)
    {
        constexpr s32 axis_index {static_cast<s32>(Axis::Z)};
        
        const f32 dz {
            actor.velocity[axis_index] <= 0.0f
                ? delta_time * FALLING_GRAVITY_MODIFIER * world.get_gravity()[axis_index]
                : delta_time * RISING_GRAVITY_MODIFIER * world.get_gravity()[axis_index]
        };
        
        actor.velocity[axis_index] = clamp(
            actor.velocity[axis_index] + dz,
            -MAX_VELOCITY, 
            MAX_VELOCITY
        );
    }

    if (actor.box_collider.collision_enabled)
    {
        const Vec3 move {
            abs(delta_time * actor.velocity.x),
            abs(delta_time * actor.velocity.y),
            abs(delta_time * actor.velocity.z),
        };

        const f32 max_move {max({move.x, move.y, move.z})};

        s32 step_count {static_cast<s32>(ceil(max_move))};
        
        if (step_count < 1)
        {
            step_count = 1;
        }

        const f32 step_delta_time {delta_time / static_cast<f32>(step_count)};

        for (s32 step_index = 0; step_index < step_count; ++step_index)
        {
            resolve_axis_collisions(world, actor, Axis::X, step_delta_time);
            resolve_axis_collisions(world, actor, Axis::Y, step_delta_time);
            resolve_axis_collisions(world, actor, Axis::Z, step_delta_time);
        }
    }
    else
    {
        const Vec3 displacement {delta_time * actor.velocity};

        actor.position = actor.position + displacement;
    }
}
