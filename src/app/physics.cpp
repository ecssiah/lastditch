#include "app/physics.h"

#include <cmath>

#include "core/types.h"
#include "app/actor.h"
#include "app/world.h"

static Bounds3 
get_box_collider_bounds(const BoxCollider& box_collider, const Vec3& position)
{
    constexpr f32 world_size = WORLD_SIZE_IN_CELLS;
    
    return {
        .min = vec3_max(position - box_collider.radius, vec3_broadcast(0.0f)),
        .max = vec3_min(position + box_collider.radius, vec3_broadcast(world_size)),
    };
}

static IBounds3 
get_grid_overlap_of_bounds(const Bounds3& bounds)
{
    return {
        .min = {
            max_s32(static_cast<s32>(floorf(bounds.min.x)), 0),
            max_s32(static_cast<s32>(floorf(bounds.min.y)), 0),
            max_s32(static_cast<s32>(floorf(bounds.min.z)), 0),
        },
        .max = {
            min_s32(static_cast<s32>(ceilf(bounds.max.x)) - 1, WORLD_SIZE_IN_CELLS - 1),
            min_s32(static_cast<s32>(ceilf(bounds.max.y)) - 1, WORLD_SIZE_IN_CELLS - 1),
            min_s32(static_cast<s32>(ceilf(bounds.max.z)) - 1, WORLD_SIZE_IN_CELLS - 1),
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
    
    for (s32 axis_index = 0; axis_index < AXIS_COUNT; ++axis_index)
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

    const IBounds3 grid_overlap_bounds = get_grid_overlap_of_bounds(swept_bounds);

    const s32 axis_index = static_cast<s32>(axis);

    const f32 actor_min_prev = actor_bounds.min[axis_index];
    const f32 actor_max_prev = actor_bounds.max[axis_index];

    const f32 actor_min_next = actor_min_prev + step_delta_time * actor.velocity[axis_index];
    const f32 actor_max_next = actor_max_prev + step_delta_time * actor.velocity[axis_index];

    const s32 axis_s = (axis_index + 1) % AXIS_COUNT;
    const s32 axis_t = (axis_index + 2) % AXIS_COUNT;

    b32 found = false;
    f32 best = actor.velocity[axis_index] > 0 ? INFINITY : -INFINITY;

    constexpr s32 axis_z_index = static_cast<s32>(Axis::Z);
    constexpr s32 axis_y_index = static_cast<s32>(Axis::Y);
    constexpr s32 axis_x_index = static_cast<s32>(Axis::X);
    
    for (s32 z = grid_overlap_bounds.min[axis_z_index]; z <= grid_overlap_bounds.max[axis_z_index]; ++z) 
    {
        for (s32 y = grid_overlap_bounds.min[axis_y_index]; y <= grid_overlap_bounds.max[axis_y_index]; ++y) 
        {
            for (s32 x = grid_overlap_bounds.min[axis_x_index]; x <= grid_overlap_bounds.max[axis_x_index]; ++x) 
            {
                const IVec3 cell_coordinate = {x, y, z};

                if (!World::cell_coordinate_is_valid(x, y, z))
                {
                    continue;
                }

                const Cell& cell = world.get_cell(x, y, z);

                if (cell.block_type == BlockType::None)
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

static void 
integrate(const f32 delta_time, Actor& actor, World& world)
{
    actor.is_grounded = false;

    if (actor.movement_type == MovementType::Ground)
    {
        constexpr s32 axis_index = static_cast<s32>(Axis::Z);
        
        f32 dz;
        
        if (actor.velocity[axis_index] <= 0.0f)
        {
            dz = delta_time * FALLING_GRAVITY_MODIFIER * GRAVITY_CONSTANT;
        }

        if (actor.velocity[axis_index] <= 0.0f)
        {
            dz = delta_time * FALLING_GRAVITY_MODIFIER * GRAVITY_CONSTANT;
        }
        else
        {
            dz = delta_time * RISING_GRAVITY_MODIFIER * GRAVITY_CONSTANT;
        }
        
        actor.velocity[axis_index] = clamp_f32(
            actor.velocity[axis_index] + dz, 
            -MAX_VELOCITY, 
            MAX_VELOCITY
        );
    }

    if (actor.box_collider.collision_enabled)
    {
        const Vec3 move = {
            fabsf(delta_time * actor.velocity.x),
            fabsf(delta_time * actor.velocity.y),
            fabsf(delta_time * actor.velocity.z),
        };

        const f32 max_move = max_f32(move.x, max_f32(move.y, move.z));

        s32 step_count = static_cast<s32>(ceilf(max_move));
        
        if (step_count < 1)
        {
            step_count = 1;
        }

        const f32 step_delta_time = delta_time / static_cast<f32>(step_count);

        for (s32 step_index = 0; step_index < step_count; ++step_index)
        {
            resolve_axis_collisions(actor, Axis::X, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::Y, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::Z, step_delta_time, world);
        }
    }
    else
    {
        const Vec3 displacement = delta_time * actor.velocity;

        actor.position = actor.position + displacement;
    }
}

void 
physics_update_actor(const f32 delta_time, Actor& actor, World& world)
{
    switch (actor.movement_type)
    {
    case MovementType::Ground: 
    case MovementType::Debug:
        integrate(delta_time, actor, world);
        break;
    }
}
