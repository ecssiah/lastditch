#include "game/sim/physics.h"

#include "core/types.h"
#include "game/sim/actor.h"
#include "game/sim/world.h"

static bounds3 
get_box_collider_bounds(const BoxCollider& box_collider, const vec3& position)
{
    constexpr f32 world_size = WORLD_SIZE_IN_CELLS;
    
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

static ibounds3 
get_grid_overlap_of_ibounds(const bounds3& bounds)
{
    return {
        {
            std::max(0, static_cast<i32>(floorf(bounds.min.x))),
            std::max(0, static_cast<i32>(floorf(bounds.min.y))),
            std::max(0, static_cast<i32>(floorf(bounds.min.z))),
        },
        {
            std::min(static_cast<i32>(ceilf(bounds.max.x)) - 1, WORLD_SIZE_IN_CELLS - 1),
            std::min(static_cast<i32>(ceilf(bounds.max.y)) - 1, WORLD_SIZE_IN_CELLS - 1),
            std::min(static_cast<i32>(ceilf(bounds.max.z)) - 1, WORLD_SIZE_IN_CELLS - 1),
        },
    };
}

static void resolve_axis_collisions(Actor& actor, axis axis, const f32 step_delta_time, World& world)
{
    if (actor.velocity[static_cast<size_t>(axis)] == 0.0f)
    {
        return;
    }

    bounds3 actor_bounds = get_box_collider_bounds(actor.box_collider, actor.position);

    bounds3 swept_bounds;
    
    for (i32 axis_index = 0; axis_index < AXIS_COUNT; ++axis_index)
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

    const ibounds3 grid_overlap_bounds = get_grid_overlap_of_ibounds(swept_bounds);

    const i32 axis_index = static_cast<size_t>(axis);

    const f32 actor_min_prev = actor_bounds.min[axis_index];
    const f32 actor_max_prev = actor_bounds.max[axis_index];

    const f32 actor_min_next = actor_min_prev + step_delta_time * actor.velocity[axis_index];
    const f32 actor_max_next = actor_max_prev + step_delta_time * actor.velocity[axis_index];

    const i32 axis_s = (axis_index + 1) % AXIS_COUNT;
    const i32 axis_t = (axis_index + 2) % AXIS_COUNT;

    b32 found = false;
    f32 best = actor.velocity[axis_index] > 0 ? INFINITY : -INFINITY;

    constexpr i32 axis_z_index = static_cast<i32>(axis::z);
    constexpr i32 axis_y_index = static_cast<i32>(axis::y);
    constexpr i32 axis_x_index = static_cast<i32>(axis::x);
    
    for (i32 z = grid_overlap_bounds.min[axis_z_index]; z <= grid_overlap_bounds.max[axis_z_index]; ++z) 
    {
        for (i32 y = grid_overlap_bounds.min[axis_y_index]; y <= grid_overlap_bounds.max[axis_y_index]; ++y) 
        {
            for (i32 x = grid_overlap_bounds.min[axis_x_index]; x <= grid_overlap_bounds.max[axis_x_index]; ++x) 
            {
                const ivec3 cell_coordinate = {x, y, z};

                const Cell* cell = world_get_cell(world, x, y, z);

                if (!cell)
                {
                    continue;
                }

                if (cell->block_type == BlockType::none)
                {
                    continue;
                }

                if (
                    actor_bounds.max[axis_s] <= cell_coordinate[axis_s] ||
                    actor_bounds.min[axis_s] >= cell_coordinate[axis_s] + 1.0f
                ) {
                    continue;
                }

                if (
                    actor_bounds.max[axis_t] <= cell_coordinate[axis_t] ||
                    actor_bounds.min[axis_t] >= cell_coordinate[axis_t] + 1.0f
                ) {
                    continue;
                }

                if (actor.velocity[axis_index] > 0)
                {
                    const f32 block_min = cell_coordinate[axis_index];

                    if (
                        block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best
                    ) {
                        best = block_min;
                        found = true;
                    }
                }
                else
                {
                    const f32 block_max = cell_coordinate[axis_index] + 1.0f;

                    if (
                        block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best
                    ) {
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

            if (axis == axis::z)
            {
                actor.is_grounded = true;
            }
        }

        actor.velocity[axis_index] = 0.0f;
    }
    else
    {
        actor.position.elements[axis_index] += step_delta_time * actor.velocity.elements[axis_index];
    }
}

static void 
integrate(Actor& actor, World& world)
{
    actor.is_grounded = false;

    if (actor.movement_type == MovementType::ground)
    {
        constexpr i32 axis_index = static_cast<i32>(axis::z);
        
        if (actor.velocity.elements[static_cast<size_t>(axis::z)] <= 0.0f)
        {
            const f32 dz = world.delta_time * FALLING_GRAVITY_MODIFIER * world.gravity.elements[axis_index];
            
            actor.velocity.elements[axis_index] = std::clamp(
                actor.velocity.elements[axis_index] + dz, 
                -MAX_VELOCITY, 
                MAX_VELOCITY
            );
        }
        else
        {
            const f32 dz = world.delta_time * RISING_GRAVITY_MODIFIER * world.gravity.elements[axis_index];
            
            actor.velocity.elements[axis_index] = std::clamp(
                actor.velocity.elements[axis_index] + dz,
                -MAX_VELOCITY,
                MAX_VELOCITY
            );
        }
    }

    if (actor.box_collider.collision_enabled)
    {
        const vec3 move = {
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
            resolve_axis_collisions(actor, axis::x, step_delta_time, world);
            resolve_axis_collisions(actor, axis::y, step_delta_time, world);
            resolve_axis_collisions(actor, axis::z, step_delta_time, world);
        }
    }
    else
    {
        const vec3 displacement = world.delta_time * actor.velocity;

        actor.position = actor.position + displacement;
    }
}

void 
physics_update_actor(Actor& actor, World& world)
{
    switch (actor.movement_type)
    {
    case MovementType::ground: 
        integrate(actor, world);
        break;
    case MovementType::debug: 
        integrate(actor, world);
        break;
    default: break;
    }
}
