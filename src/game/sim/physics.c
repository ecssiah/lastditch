#include "game/sim/physics.h"

#include "jsk.h"

#include "core/core_data.h"
#include "game/sim/sim_data.h"
#include "game/sim/world.h"

#include <math.h>

static void get_int_bounds(BoxCollider *box_collider, vec3 position, IntBounds *out_int_bounds)
{
    out_int_bounds->min[0] = position[0] - box_collider->radius[0];
    out_int_bounds->min[1] = position[1] - box_collider->radius[1];
    out_int_bounds->min[2] = position[2] - box_collider->radius[2];

    out_int_bounds->max[0] = position[0] + box_collider->radius[0];
    out_int_bounds->max[1] = position[1] + box_collider->radius[1];
    out_int_bounds->max[2] = position[2] + box_collider->radius[2];

    i32 axis_index;
    for (axis_index = 0; axis_index < AXIS_COUNT; ++axis_index)
    {
        if (out_int_bounds->min[axis_index] < 0)
        {
            out_int_bounds->min[axis_index] = 0;
        }
        
        if (out_int_bounds->max[axis_index] >= WORLD_SIZE_IN_CELLS)
        {
            out_int_bounds->max[axis_index] = WORLD_SIZE_IN_CELLS - 1;
        }
    }
}

static void get_float_bounds(BoxCollider *box_collider, vec3 position, FloatBounds *out_float_bounds)
{
    out_float_bounds->min[0] = position[0] - box_collider->radius[0];
    out_float_bounds->min[1] = position[1] - box_collider->radius[1];
    out_float_bounds->min[2] = position[2] - box_collider->radius[2];

    out_float_bounds->max[0] = position[0] + box_collider->radius[0];
    out_float_bounds->max[1] = position[1] + box_collider->radius[1];
    out_float_bounds->max[2] = position[2] + box_collider->radius[2];

    const f32 world_size = (f32)WORLD_SIZE_IN_CELLS;

    i32 axis_index;
    for (axis_index = 0; axis_index < AXIS_COUNT; ++axis_index)
    {
        if (out_float_bounds->min[axis_index] < 0.0f)
        {
            out_float_bounds->min[axis_index] = 0.0f;
        }
        
        if (out_float_bounds->max[axis_index] > world_size)
        {
            out_float_bounds->max[axis_index] = world_size;
        }
    }
}

static void get_overlap_bounds(BoxCollider *box_collider, vec3 position, IntBounds *out_int_bounds)
{
    out_int_bounds->min[0] = (i32)floorf(position[0] - box_collider->radius[0]);
    out_int_bounds->min[1] = (i32)floorf(position[1] - box_collider->radius[1]);
    out_int_bounds->min[2] = (i32)floorf(position[2] - box_collider->radius[2]);

    out_int_bounds->max[0] = (i32)ceilf(position[0] + box_collider->radius[0]) - 1;
    out_int_bounds->max[1] = (i32)ceilf(position[1] + box_collider->radius[1]) - 1;
    out_int_bounds->max[2] = (i32)ceilf(position[2] + box_collider->radius[2]) - 1;

    out_int_bounds->min[0] = out_int_bounds->min[0] < 0 ? 0 : out_int_bounds->min[0];
    out_int_bounds->min[1] = out_int_bounds->min[1] < 0 ? 0 : out_int_bounds->min[1];
    out_int_bounds->min[2] = out_int_bounds->min[2] < 0 ? 0 : out_int_bounds->min[2];

    out_int_bounds->max[0] = out_int_bounds->max[0] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_int_bounds->max[0];
    out_int_bounds->max[1] = out_int_bounds->max[1] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_int_bounds->max[1];
    out_int_bounds->max[2] = out_int_bounds->max[2] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_int_bounds->max[2];
}

static void get_overlap_bounds_from_float_bounds(FloatBounds *float_bounds, IntBounds *out_int_bounds)
{
    out_int_bounds->min[0] = (i32)floorf(float_bounds->min[0]);
    out_int_bounds->min[1] = (i32)floorf(float_bounds->min[1]);
    out_int_bounds->min[2] = (i32)floorf(float_bounds->min[2]);

    out_int_bounds->max[0] = (i32)ceilf(float_bounds->max[0]) - 1;
    out_int_bounds->max[1] = (i32)ceilf(float_bounds->max[1]) - 1;
    out_int_bounds->max[2] = (i32)ceilf(float_bounds->max[2]) - 1;

    if (out_int_bounds->min[0] < 0) out_int_bounds->min[0] = 0;
    if (out_int_bounds->min[1] < 0) out_int_bounds->min[1] = 0;
    if (out_int_bounds->min[2] < 0) out_int_bounds->min[2] = 0;

    if (out_int_bounds->max[0] >= WORLD_SIZE_IN_CELLS) out_int_bounds->max[0] = WORLD_SIZE_IN_CELLS - 1;
    if (out_int_bounds->max[1] >= WORLD_SIZE_IN_CELLS) out_int_bounds->max[1] = WORLD_SIZE_IN_CELLS - 1;
    if (out_int_bounds->max[2] >= WORLD_SIZE_IN_CELLS) out_int_bounds->max[2] = WORLD_SIZE_IN_CELLS - 1;
}

static void integrate_axis(Sim *sim, Actor *actor, Axis axis, f32 delta_time)
{           
    if (actor->velocity[axis] == 0.0f)
    {
        return;
    }

    FloatBounds actor_bounds;
    get_float_bounds(&actor->box_collider, actor->position, &actor_bounds);

    FloatBounds swept_bounds;

    i32 axis_index;
    for (axis_index = 0; axis_index < 3; ++axis_index)
    {
        swept_bounds.min[axis_index] = fminf(
            actor_bounds.min[axis_index],
            actor_bounds.min[axis_index] + delta_time * actor->velocity[axis_index]
        );
        
        swept_bounds.max[axis_index] = fmaxf(
            actor_bounds.max[axis_index],
            actor_bounds.max[axis_index] + delta_time * actor->velocity[axis_index]
        );
    }
    
    IntBounds overlap_bounds;
    get_overlap_bounds_from_float_bounds(&swept_bounds, &overlap_bounds);

    const f32 actor_min_prev = actor_bounds.min[axis];
    const f32 actor_max_prev = actor_bounds.max[axis];

    const f32 actor_min_next = actor_min_prev + delta_time * actor->velocity[axis];
    const f32 actor_max_next = actor_max_prev + delta_time * actor->velocity[axis];

    const i32 axis_s = (axis + 1) % AXIS_COUNT;
    const i32 axis_t = (axis + 2) % AXIS_COUNT;
    
    b32 found = FALSE;
    f32 best = actor->velocity[axis] > 0 ? INFINITY : -INFINITY;
    
    i32 x, y, z;

    for (z = overlap_bounds.min[AXIS_Z]; z <= overlap_bounds.max[AXIS_Z]; ++z)
    {
        for (y = overlap_bounds.min[AXIS_Y]; y <= overlap_bounds.max[AXIS_Y]; ++y)
        {
            for (x = overlap_bounds.min[AXIS_X]; x <= overlap_bounds.max[AXIS_X]; ++x)
            {
                const ivec3 cell_coordinate = { x, y, z };
                    
                const Cell *cell = world_get_cell(sim, x, y, z);

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
                ) {
                    continue;
                }
                    
                if (
                    actor_bounds.max[axis_t] <= cell_coordinate[axis_t] ||
                    actor_bounds.min[axis_t] >= cell_coordinate[axis_t] + 1.0f
                ) {
                    continue;
                }

                if (actor->velocity[axis] > 0)
                {
                    const f32 block_min = cell_coordinate[axis];

                    if (
                        block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best
                    ) {
                        best = block_min;
                        found = TRUE;
                    }
                }
                else
                {
                    const f32 block_max = cell_coordinate[axis] + 1.0f;

                    if (
                        block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best
                    ) {
                        best = block_max;
                        found = TRUE;
                    }
                }
            }
        }
    }

    if (found)
    {
        if (actor->velocity[axis] > 0)
        {
            actor->position[axis] = best - actor->box_collider.radius[axis];
        }
        else
        {
            actor->position[axis] = best + actor->box_collider.radius[axis];

            if (axis == AXIS_Z)
            {
                actor->is_grounded = TRUE;
            }
        }

        actor->velocity[axis] = 0.0f;
    }
    else
    {
        actor->position[axis] += delta_time * actor->velocity[axis];
    }
}

void physics_integrate(Sim *sim, Actor *actor)
{
    actor->is_grounded = FALSE;
    
    if (actor->movement_type == MOVEMENT_TYPE_GROUND)
    {
        if (actor->velocity[AXIS_Z] < 0.0f)
        {
            actor->velocity[AXIS_Z] += sim->time.delta_time * FALLING_GRAVITY_MODIFIER * sim->physics.gravity[AXIS_Z];
        }
        else
        {
            actor->velocity[AXIS_Z] += sim->time.delta_time * RISING_GRAVITY_MODIFIER * sim->physics.gravity[AXIS_Z];
        }
    }

    const f32 move_x = fabsf(sim->time.delta_time * actor->velocity[0]);
    const f32 move_y = fabsf(sim->time.delta_time * actor->velocity[1]);
    const f32 move_z = fabsf(sim->time.delta_time * actor->velocity[2]);

    const f32 max_move = fmaxf(move_x, fmaxf(move_y, move_z));

    i32 step_count = (i32)ceilf(max_move);
    if (step_count < 1)
    {
        step_count = 1;
    }

    const f32 step_delta_time = sim->time.delta_time / (f32)step_count;

    i32 step_index;
    for (step_index = 0; step_index < step_count; ++step_index)
    {
        integrate_axis(sim, actor, AXIS_X, step_delta_time);
        integrate_axis(sim, actor, AXIS_Y, step_delta_time);
        integrate_axis(sim, actor, AXIS_Z, step_delta_time);
    }
}
