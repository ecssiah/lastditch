#include "game/sim/physics.h"

#include "core/types.h"
#include "core/math_ext.h"
#include "game/sim/actor.h"
#include "game/sim/world.h"

static Bounds3f 
get_bounds(const BoxCollider& box_collider, const glm::vec3 position)
{
    Bounds3f bounds_result = {
        {
            position.x - box_collider.radius[0],
            position.y - box_collider.radius[1],
            position.z - box_collider.radius[2],
        },
        {
            position.x + box_collider.radius[0],
            position.y + box_collider.radius[1],
            position.z + box_collider.radius[2],
        },
    };

    constexpr f32 world_size = WORLD_SIZE_IN_CELLS;

    for (i32 axis_index = 0; axis_index < 3; ++axis_index)
    {
        if (bounds_result.min[axis_index] < 0.0f)
        {
            bounds_result.min[axis_index] = 0.0f;
        }

        if (bounds_result.max[axis_index] > world_size)
        {
            bounds_result.max[axis_index] = world_size;
        }
    }
    
    return bounds_result;
}

static Bounds3i 
get_grid_overlap_from_bounds(const Bounds3f& bounds)
{
    Bounds3i bounds_result = {
        {
            static_cast<i32>(floorf(bounds.min.x)),
            static_cast<i32>(floorf(bounds.min.y)),
            static_cast<i32>(floorf(bounds.min.z)),
        },
        {
            static_cast<i32>(ceilf(bounds.max.x)) - 1,
            static_cast<i32>(ceilf(bounds.max.y)) - 1,
            static_cast<i32>(ceilf(bounds.max.z)) - 1,
        },
    }; 

    if (bounds_result.min.x < 0) bounds_result.min.x = 0;
    if (bounds_result.min.y < 0) bounds_result.min.y = 0;
    if (bounds_result.min.z < 0) bounds_result.min.z = 0;

    if (bounds_result.max.x >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) bounds_result.max.x = WORLD_SIZE_IN_CELLS - 1;
    if (bounds_result.max.y >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) bounds_result.max.y = WORLD_SIZE_IN_CELLS - 1;
    if (bounds_result.max.z >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) bounds_result.max.z = WORLD_SIZE_IN_CELLS - 1;
    
    return bounds_result;
}

static void resolve_axis_collisions(Actor& actor, Axis axis, f32 step_delta_time, World& world)
{
    if (actor.velocity[static_cast<size_t>(axis)] == 0.0f)
    {
        return;
    }

    Bounds3f actor_bounds = get_bounds(actor.box_collider, actor.position);

    Bounds3f swept_bounds;

    for (i32 axis_index = 0; axis_index < 3; ++axis_index)
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

    const Bounds3i grid_overlap_bounds = get_grid_overlap_from_bounds(swept_bounds);

    const size_t axis_index = static_cast<size_t>(axis);

    const f32 actor_min_prev = actor_bounds.min[axis_index];
    const f32 actor_max_prev = actor_bounds.max[axis_index];

    const f32 actor_min_next = actor_min_prev + step_delta_time * actor.velocity[axis_index];
    const f32 actor_max_next = actor_max_prev + step_delta_time * actor.velocity[axis_index];

    const i32 axis_s = (axis_index + 1) % 3;
    const i32 axis_t = (axis_index + 2) % 3;

    b32 found = false;
    f32 best = actor.velocity[axis_index] > 0 ? INFINITY : -INFINITY;

    for (
        i32 z = grid_overlap_bounds.min[static_cast<size_t>(Axis::z)]; 
        z <= grid_overlap_bounds.max[static_cast<size_t>(Axis::z)]; 
        ++z
    ) {
        for (
            i32 y = grid_overlap_bounds.min[static_cast<size_t>(Axis::y)]; 
            y <= grid_overlap_bounds.max[static_cast<size_t>(Axis::y)]; 
            ++y
        ) {
            for (
                i32 x = grid_overlap_bounds.min[static_cast<size_t>(Axis::x)]; 
                x <= grid_overlap_bounds.max[static_cast<size_t>(Axis::x)]; 
                ++x
            ) {
                const glm::ivec3 cell_coordinate = {x, y, z};

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

    if (actor.movement_type == MovementType::ground)
    {
        constexpr i32 axis_index = static_cast<i32>(Axis::z);
        
        if (actor.velocity[static_cast<size_t>(Axis::z)] <= 0.0f)
        {
            const f32 dz = world.delta_time * FALLING_GRAVITY_MODIFIER * world.gravity[axis_index];
            
            actor.velocity[axis_index] = glm::clamp(
                actor.velocity[axis_index] + dz, 
                -MAX_VELOCITY, 
                MAX_VELOCITY
            );
        }
        else
        {
            const f32 dz = world.delta_time * RISING_GRAVITY_MODIFIER * world.gravity[axis_index];
            
            actor.velocity[axis_index] = glm::clamp(
                actor.velocity[axis_index] + dz,
                -MAX_VELOCITY,
                MAX_VELOCITY
            );
        }
    }

    if (actor.box_collider.collision_enabled)
    {
        const glm::vec3 move = {
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
        const glm::vec3 displacement = world.delta_time * actor.velocity;

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
