#include "game/sim/physics.h"

#include "core/types.h"
#include "core/math_ext.h"
#include "game/sim/actor.h"
#include "game/sim/world.h"

static void get_fbounds(BoxCollider* box_collider, vec3 position, Bounds3f* out_fbounds)
{
    out_fbounds->min[0] = position[0] - box_collider->radius[0];
    out_fbounds->min[1] = position[1] - box_collider->radius[1];
    out_fbounds->min[2] = position[2] - box_collider->radius[2];

    out_fbounds->max[0] = position[0] + box_collider->radius[0];
    out_fbounds->max[1] = position[1] + box_collider->radius[1];
    out_fbounds->max[2] = position[2] + box_collider->radius[2];

    constexpr f32 world_size = WORLD_SIZE_IN_CELLS;

    for (u32 axis_index = 0; axis_index < 3; ++axis_index)
    {
        if (out_fbounds->min[axis_index] < 0.0f)
        {
            out_fbounds->min[axis_index] = 0.0f;
        }

        if (out_fbounds->max[axis_index] > world_size)
        {
            out_fbounds->max[axis_index] = world_size;
        }
    }
}

static void get_overlap_from_fbounds(Bounds3f* fbounds, Bounds3i* out_ibounds)
{
    out_ibounds->min[0] = static_cast<i32>(floorf(fbounds->min[0]));
    out_ibounds->min[1] = static_cast<i32>(floorf(fbounds->min[1]));
    out_ibounds->min[2] = static_cast<i32>(floorf(fbounds->min[2]));

    out_ibounds->max[0] = static_cast<i32>(ceilf(fbounds->max[0])) - 1;
    out_ibounds->max[1] = static_cast<i32>(ceilf(fbounds->max[1])) - 1;
    out_ibounds->max[2] = static_cast<i32>(ceilf(fbounds->max[2])) - 1;

    if (out_ibounds->min[0] < 0) out_ibounds->min[0] = 0;
    if (out_ibounds->min[1] < 0) out_ibounds->min[1] = 0;
    if (out_ibounds->min[2] < 0) out_ibounds->min[2] = 0;

    if (out_ibounds->max[0] >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) out_ibounds->max[0] = WORLD_SIZE_IN_CELLS - 1;
    if (out_ibounds->max[1] >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) out_ibounds->max[1] = WORLD_SIZE_IN_CELLS - 1;
    if (out_ibounds->max[2] >= static_cast<i32>(WORLD_SIZE_IN_CELLS)) out_ibounds->max[2] = WORLD_SIZE_IN_CELLS - 1;
}

static void resolve_axis_collisions(Actor* actor, Axis axis, f32 step_delta_time, World* world)
{
    if (actor->velocity[static_cast<size_t>(axis)] == 0.0f)
    {
        return;
    }

    Bounds3f actor_bounds;
    get_fbounds(&actor->box_collider, actor->position, &actor_bounds);

    Bounds3f swept_bounds;

    for (u32 axis_index = 0; axis_index < 3; ++axis_index)
    {
        swept_bounds.min[axis_index] = fminf(
            actor_bounds.min[axis_index],
            actor_bounds.min[axis_index] + step_delta_time * actor->velocity[axis_index]
        );

        swept_bounds.max[axis_index] = fmaxf(
            actor_bounds.max[axis_index],
            actor_bounds.max[axis_index] + step_delta_time * actor->velocity[axis_index]
        );
    }

    Bounds3i overlap_bounds;
    get_overlap_from_fbounds(&swept_bounds, &overlap_bounds);

    const size_t AXIS_VALUE = static_cast<size_t>(axis);

    const f32 actor_min_prev = actor_bounds.min[AXIS_VALUE];
    const f32 actor_max_prev = actor_bounds.max[AXIS_VALUE];

    const f32 actor_min_next = actor_min_prev + step_delta_time * actor->velocity[AXIS_VALUE];
    const f32 actor_max_next = actor_max_prev + step_delta_time * actor->velocity[AXIS_VALUE];

    const i32 axis_s = (AXIS_VALUE + 1) % 3;
    const i32 axis_t = (AXIS_VALUE + 2) % 3;

    b32 found = false;
    f32 best = actor->velocity[AXIS_VALUE] > 0 ? INFINITY : -INFINITY;

    for (i32 z = overlap_bounds.min[static_cast<size_t>(Axis::z)]; z <= overlap_bounds.max[static_cast<size_t>(Axis::z)]
         ; ++z)
    {
        for (i32 y = overlap_bounds.min[static_cast<size_t>(Axis::y)]; y <= overlap_bounds.max[static_cast<size_t>(
                 Axis::y)]; ++y)
        {
            for (i32 x = overlap_bounds.min[static_cast<size_t>(Axis::x)]; x <= overlap_bounds.max[static_cast<size_t>(
                     Axis::x)]; ++x)
            {
                const ivec3 cell_coordinate = {x, y, z};

                const Cell* cell = world_get_cell(world, x, y, z);

                if (!cell)
                {
                    continue;
                }

                if (cell->block_type == BLOCK_TYPE_NONE)
                {
                    continue;
                }

                if (
                    actor_bounds.max[axis_s] <= cell_coordinate[axis_s] ||
                    actor_bounds.min[axis_s] >= cell_coordinate[axis_s] + 1.0f
                )
                {
                    continue;
                }

                if (
                    actor_bounds.max[axis_t] <= cell_coordinate[axis_t] ||
                    actor_bounds.min[axis_t] >= cell_coordinate[axis_t] + 1.0f
                )
                {
                    continue;
                }

                if (actor->velocity[AXIS_VALUE] > 0)
                {
                    const f32 block_min = cell_coordinate[AXIS_VALUE];

                    if (
                        block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best
                    )
                    {
                        best = block_min;
                        found = true;
                    }
                }
                else
                {
                    const f32 block_max = cell_coordinate[AXIS_VALUE] + 1.0f;

                    if (
                        block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best
                    )
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
        if (actor->velocity[AXIS_VALUE] > 0)
        {
            actor->position[AXIS_VALUE] = best - actor->box_collider.radius[AXIS_VALUE];
        }
        else
        {
            actor->position[AXIS_VALUE] = best + actor->box_collider.radius[AXIS_VALUE];

            if (axis == Axis::z)
            {
                actor->is_grounded = true;
            }
        }

        actor->velocity[AXIS_VALUE] = 0.0f;
    }
    else
    {
        actor->position[AXIS_VALUE] += step_delta_time * actor->velocity[AXIS_VALUE];
    }
}

static void integrate(Actor* actor, World* world)
{
    actor->is_grounded = false;

    if (actor->movement_type == MOVEMENT_TYPE_GROUND)
    {
        if (actor->velocity[static_cast<size_t>(Axis::z)] <= 0.0f)
        {
            actor->velocity[static_cast<size_t>(Axis::z)] += world->delta_time * FALLING_GRAVITY_MODIFIER * world->
                gravity[static_cast<size_t>(Axis::z)];
        }
        else
        {
            actor->velocity[static_cast<size_t>(Axis::z)] += world->delta_time * RISING_GRAVITY_MODIFIER * world->
                gravity[static_cast<size_t>(Axis::z)];
        }
    }

    if (actor->box_collider.collision_enabled)
    {
        const vec3 move = {
            fabsf(world->delta_time * actor->velocity[0]),
            fabsf(world->delta_time * actor->velocity[1]),
            fabsf(world->delta_time * actor->velocity[2]),
        };

        const f32 max_move = fmaxf(move[0], fmaxf(move[1], move[2]));

        u32 step_count = static_cast<u32>(ceilf(max_move));
        if (step_count < 1)
        {
            step_count = 1;
        }

        const f32 step_delta_time = world->delta_time / step_count;

        for (u32 step_index = 0; step_index < step_count; ++step_index)
        {
            resolve_axis_collisions(actor, Axis::x, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::y, step_delta_time, world);
            resolve_axis_collisions(actor, Axis::z, step_delta_time, world);
        }
    }
    else
    {
        vec3 displacement;
        glm_vec3_scale(actor->velocity, world->delta_time, displacement);

        glm_vec3_add(actor->position, displacement, actor->position);
    }
}

void physics_update_actor(Actor* actor, World* world)
{
    switch (actor->movement_type)
    {
    case MOVEMENT_TYPE_GROUND: integrate(actor, world);
        break;
    case MOVEMENT_TYPE_DEBUG: integrate(actor, world);
        break;
    default: break;
    }
}
