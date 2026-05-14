#include "app/physics.h"

#include "core/types.h"
#include "app/actor.h"
#include "app/world.h"

static Bounds3 
get_box_collider_bounds(const BoxCollider& box_collider, const Vec3& position)
{
    constexpr f32 world_size = world_size_in_cells;
    
    return {
        {
            std::max(0.0f, position.x - box_collider.radius.x),
            std::max(0.0f, position.y - box_collider.radius.y),
            std::max(0.0f, position.z - box_collider.radius.z)
        },
        {
            std::min(position.x + box_collider.radius.x, world_size),
            std::min(position.y + box_collider.radius.y, world_size),
            std::min(position.z + box_collider.radius.z, world_size),
        },
    };
}

static IBounds3 
get_grid_overlap_of_ibounds(const Bounds3& bounds)
{
    return {
        {
            std::max(0, static_cast<i32>(floorf(bounds.min.x))),
            std::max(0, static_cast<i32>(floorf(bounds.min.y))),
            std::max(0, static_cast<i32>(floorf(bounds.min.z))),
        },
        {
            std::min(static_cast<i32>(ceilf(bounds.max.x)) - 1, world_size_in_cells - 1),
            std::min(static_cast<i32>(ceilf(bounds.max.y)) - 1, world_size_in_cells - 1),
            std::min(static_cast<i32>(ceilf(bounds.max.z)) - 1, world_size_in_cells - 1),
        },
    };
}

static void 
resolve_axis_collisions(Actor& actor, Axis axis, const f32 step_delta_time, World& world)
{
    if (actor.velocity[static_cast<size_t>(axis)] == 0.0f)
    {
        return;
    }

    Bounds3 actor_bounds = get_box_collider_bounds(actor.box_collider, actor.position);

    Bounds3 swept_bounds;
    
    for (i32 axis_index = 0; axis_index < axis_count; ++axis_index)
    {
        swept_bounds.min[axis_index] = fminf(
            actor_bounds.min[axis_index],
            actor_bounds.min[axis_index] + step_delta_time * actor.velocity[axis_index]
        );

        swept_bounds.max[axis_index] = fmaxf(
            actor_bounds.max[axis_index],
            actor_bounds.max[axis_index] + step_delta_time * actor.velocity[axis_index]
        );
    }

    const IBounds3 grid_overlap_bounds = get_grid_overlap_of_ibounds(swept_bounds);

    const i32 axis_index = static_cast<i32>(axis);

    const f32 actor_min_prev = actor_bounds.min[axis_index];
    const f32 actor_max_prev = actor_bounds.max[axis_index];

    const f32 actor_min_next = actor_min_prev + step_delta_time * actor.velocity[axis_index];
    const f32 actor_max_next = actor_max_prev + step_delta_time * actor.velocity[axis_index];

    const i32 axis_s = (axis_index + 1) % axis_count;
    const i32 axis_t = (axis_index + 2) % axis_count;

    b32 found = false;
    f32 best = actor.velocity[axis_index] > 0 ? INFINITY : -INFINITY;

    constexpr i32 axis_z_index = static_cast<i32>(Axis::z);
    constexpr i32 axis_y_index = static_cast<i32>(Axis::y);
    constexpr i32 axis_x_index = static_cast<i32>(Axis::x);
    
    for (i32 z = grid_overlap_bounds.min[axis_z_index]; z <= grid_overlap_bounds.max[axis_z_index]; ++z) 
    {
        for (i32 y = grid_overlap_bounds.min[axis_y_index]; y <= grid_overlap_bounds.max[axis_y_index]; ++y) 
        {
            for (i32 x = grid_overlap_bounds.min[axis_x_index]; x <= grid_overlap_bounds.max[axis_x_index]; ++x) 
            {
                const IVec3 cell_coordinate = {x, y, z};

                const cell* cell = world_get_cell(world, x, y, z);

                if (!cell)
                {
                    continue;
                }

                if (cell->block_type == BlockType::None)
                {
                    continue;
                }

                if (actor_bounds.max[axis_s] <= cell_coordinate[axis_s] || actor_bounds.min[axis_s] >= cell_coordinate[axis_s] + 1.0f) 
                {
                    continue;
                }

                if (actor_bounds.max[axis_t] <= cell_coordinate[axis_t] || actor_bounds.min[axis_t] >= cell_coordinate[axis_t] + 1.0f) 
                {
                    continue;
                }

                if (actor.velocity[axis_index] > 0)
                {
                    const f32 block_min = cell_coordinate[axis_index];

                    if (block_min >= actor_max_prev && block_min <= actor_max_next && block_min < best) 
                    {
                        best = block_min;
                        found = true;
                    }
                }
                else
                {
                    const f32 block_max = cell_coordinate[axis_index] + 1.0f;

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

            if (axis == Axis::z)
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

static void 
integrate(Actor& actor, World& world)
{
    actor.is_grounded = false;

    if (actor.movement_type == MovementType::Ground)
    {
        constexpr i32 axis_index = static_cast<i32>(Axis::z);
        
        f32 dz;
        
        if (actor.velocity[axis_index] <= 0.0f)
        {
            dz = world.delta_time * falling_gravity_modifier * world.gravity[axis_index];
        }
        else
        {
            dz = world.delta_time * rising_gravity_modifier * world.gravity[axis_index];
        }
        
        actor.velocity[axis_index] = std::clamp(
            actor.velocity[axis_index] + dz, 
            -max_velocity, 
            max_velocity
        );
    }

    if (actor.box_collider.collision_enabled)
    {
        const Vec3 move = {
            fabsf(world.delta_time * actor.velocity.x),
            fabsf(world.delta_time * actor.velocity.y),
            fabsf(world.delta_time * actor.velocity.z),
        };

        const f32 max_move = fmaxf(move.x, fmaxf(move.y, move.z));

        i32 step_count = static_cast<i32>(ceilf(max_move));
        
        if (step_count < 1)
        {
            step_count = 1;
        }

        const f32 step_delta_time = world.delta_time / step_count;

        for (i32 step_index = 0; step_index < step_count; ++step_index)
        {
            resolve_axis_collisions(actor, Axis::x, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::y, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::z, step_delta_time, world);
        }
    }
    else
    {
        const Vec3 displacement = world.delta_time * actor.velocity;

        actor.position = actor.position + displacement;
    }
}

void 
physics_update_actor(Actor& actor, World& world)
{
    switch (actor.movement_type)
    {
    case MovementType::Ground: 
        integrate(actor, world);
        break;
    case MovementType::Debug: 
        integrate(actor, world);
        break;
    }
}
