#include "physics.h"
#include "jsk.h"
#include "jsk_log.h"
#include "ld_data.h"
#include "world.h"

#include <math.h>

static void get_int_bounds(BoxCollider *box_collider, IntBounds *out_int_bounds)
{
    out_int_bounds->min[0] = box_collider->position[0] - box_collider->radius[0];
    out_int_bounds->min[1] = box_collider->position[1] - box_collider->radius[1];
    out_int_bounds->min[2] = box_collider->position[2] - box_collider->radius[2];

    out_int_bounds->max[0] = box_collider->position[0] + box_collider->radius[0];
    out_int_bounds->max[1] = box_collider->position[1] + box_collider->radius[1];
    out_int_bounds->max[2] = box_collider->position[2] + box_collider->radius[2];
}

static void get_float_bounds(BoxCollider *box_collider, FloatBounds *out_fbounds)
{
    out_fbounds->min[0] = box_collider->position[0] - box_collider->radius[0];
    out_fbounds->min[1] = box_collider->position[1] - box_collider->radius[1];
    out_fbounds->min[2] = box_collider->position[2] - box_collider->radius[2];

    out_fbounds->max[0] = box_collider->position[0] + box_collider->radius[0];
    out_fbounds->max[1] = box_collider->position[1] + box_collider->radius[1];
    out_fbounds->max[2] = box_collider->position[2] + box_collider->radius[2];
}

static void get_overlap_bounds(BoxCollider *box_collider, IntBounds *out_int_bounds)
{
    out_int_bounds->min[0] = (i32)floorf(box_collider->position[0] - box_collider->radius[0]);
    out_int_bounds->min[1] = (i32)floorf(box_collider->position[1] - box_collider->radius[1]);
    out_int_bounds->min[2] = (i32)floorf(box_collider->position[2] - box_collider->radius[2]);

    out_int_bounds->max[0] = (i32)ceilf(box_collider->position[0] + box_collider->radius[0]) - 1;
    out_int_bounds->max[1] = (i32)ceilf(box_collider->position[1] + box_collider->radius[1]) - 1;
    out_int_bounds->max[2] = (i32)ceilf(box_collider->position[2] + box_collider->radius[2]) - 1;

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

static void get_colliding_cell_set(Sim *sim, Actor *actor)
{
    IntBounds overlap_bounds;
    get_overlap_bounds(&actor->box_collider, &overlap_bounds);
    
    sim->physics.overlap_cell_set.count = 0;

    i32 x, y, z;
    for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
    {
        for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
        {
            for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
            {
                if (world_is_solid(sim, x, y, z))
                {
                    const i32 cell_index = world_cell_coordinate_to_index(x, y, z);
                    const u32 cell_set_index = sim->physics.overlap_cell_set.count;
                    
                    sim->physics.overlap_cell_set.cell_ptr_array[cell_set_index] = &sim->cell_array[cell_index];

                    sim->physics.overlap_cell_set.count += 1;
                }
            }
        }
    }
}

static void integrate_x(Sim *sim, Actor *actor)
{
    if (actor->velocity[0] == 0.0f)
    {
        return;
    }

    FloatBounds actor_bounds;
    get_float_bounds(&actor->box_collider, &actor_bounds);

    FloatBounds swept_bounds = actor_bounds;
    swept_bounds.min[0] = fminf(actor_bounds.min[0], actor_bounds.min[0] + actor->velocity[0]);
    swept_bounds.max[0] = fmaxf(actor_bounds.max[0], actor_bounds.max[0] + actor->velocity[0]);

    IntBounds overlap_bounds;
    get_overlap_bounds_from_float_bounds(&swept_bounds, &overlap_bounds);

    f32 actor_min_prev = actor_bounds.min[0];
    f32 actor_max_prev = actor_bounds.max[0];

    f32 actor_min_next = actor_min_prev + actor->velocity[0];
    f32 actor_max_next = actor_max_prev + actor->velocity[0];

    i32 x, y, z;

    if (actor->velocity[0] > 0)
    {
        b32 found = FALSE;
        f32 best = INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[1] <= y || actor_bounds.min[1] >= y + 1.0f)
                        continue;

                    if (actor_bounds.max[2] <= z || actor_bounds.min[2] >= z + 1.0f)
                        continue;

                    f32 block_min = x;

                    if (block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best)
                    {
                        best = block_min;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[0] = best - actor->box_collider.radius[0];
            actor->velocity[0] = 0.0f;
        }
        else
        {
            actor->position[0] += actor->velocity[0];
        }
    }
    else
    {
        b32 found = FALSE;
        f32 best = -INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[1] <= y || actor_bounds.min[1] >= y + 1.0f)
                        continue;

                    if (actor_bounds.max[2] <= z || actor_bounds.min[2] >= z + 1.0f)
                        continue;

                    f32 block_max = x + 1.0f;

                    if (block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best)
                    {
                        best = block_max;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[0] = best + actor->box_collider.radius[0];
            actor->velocity[0] = 0.0f;
        }
        else
        {
            actor->position[0] += actor->velocity[0];
        }
    }
}

static void integrate_y(Sim *sim, Actor *actor)
{
    if (actor->velocity[1] == 0.0f)
    {
        return;
    }

    FloatBounds actor_bounds;
    get_float_bounds(&actor->box_collider, &actor_bounds);

    FloatBounds swept_bounds = actor_bounds;
    swept_bounds.min[1] = fminf(actor_bounds.min[1], actor_bounds.min[1] + actor->velocity[1]);
    swept_bounds.max[1] = fmaxf(actor_bounds.max[1], actor_bounds.max[1] + actor->velocity[1]);

    IntBounds overlap_bounds;
    get_overlap_bounds_from_float_bounds(&swept_bounds, &overlap_bounds);

    f32 actor_min_prev = actor_bounds.min[1];
    f32 actor_max_prev = actor_bounds.max[1];

    f32 actor_min_next = actor_min_prev + actor->velocity[1];
    f32 actor_max_next = actor_max_prev + actor->velocity[1];

    i32 x, y, z;

    if (actor->velocity[1] > 0)
    {
        b32 found = FALSE;
        f32 best = INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[2] <= z || actor_bounds.min[2] >= z + 1.0f)
                        continue;

                    if (actor_bounds.max[0] <= x || actor_bounds.min[0] >= x + 1.0f)
                        continue;

                    f32 block_min = y;

                    if (block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best)
                    {
                        best = block_min;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[1] = best - actor->box_collider.radius[1];
            actor->velocity[1] = 0.0f;
        }
        else
        {
            actor->position[1] += actor->velocity[1];
        }
    }
    else
    {
        b32 found = FALSE;
        f32 best = -INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[2] <= z || actor_bounds.min[2] >= z + 1.0f)
                        continue;

                    if (actor_bounds.max[0] <= x || actor_bounds.min[0] >= x + 1.0f)
                        continue;

                    f32 block_max = y + 1.0f;

                    if (block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best)
                    {
                        best = block_max;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[1] = best + actor->box_collider.radius[1];
            actor->velocity[1] = 0.0f;
        }
        else
        {
            actor->position[1] += actor->velocity[1];
        }
    }
}

static void integrate_z(Sim *sim, Actor *actor)
{
    if (actor->velocity[2] == 0.0f)
    {
        return;
    }

    FloatBounds actor_bounds;
    get_float_bounds(&actor->box_collider, &actor_bounds);

    FloatBounds swept_bounds = actor_bounds;
    swept_bounds.min[2] = fminf(actor_bounds.min[2], actor_bounds.min[2] + actor->velocity[2]);
    swept_bounds.max[2] = fmaxf(actor_bounds.max[2], actor_bounds.max[2] + actor->velocity[2]);

    IntBounds overlap_bounds;
    get_overlap_bounds_from_float_bounds(&swept_bounds, &overlap_bounds);

    f32 actor_min_prev = actor_bounds.min[2];
    f32 actor_max_prev = actor_bounds.max[2];

    f32 actor_min_next = actor_min_prev + actor->velocity[2];
    f32 actor_max_next = actor_max_prev + actor->velocity[2];

    i32 x, y, z;

    if (actor->velocity[2] > 0)
    {
        b32 found = FALSE;
        f32 best = INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[0] <= x || actor_bounds.min[0] >= x + 1.0f)
                        continue;

                    if (actor_bounds.max[1] <= y || actor_bounds.min[1] >= y + 1.0f)
                        continue;

                    f32 block_min = z;

                    if (block_min >= actor_max_prev &&
                        block_min <= actor_max_next &&
                        block_min < best)
                    {
                        best = block_min;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[2] = best - actor->box_collider.radius[2];
            actor->velocity[2] = 0.0f;
        }
        else
        {
            actor->position[2] += actor->velocity[2];
        }
    }
    else
    {
        b32 found = FALSE;
        f32 best = -INFINITY;

        for (z = overlap_bounds.min[2]; z <= overlap_bounds.max[2]; ++z)
        {
            for (y = overlap_bounds.min[1]; y <= overlap_bounds.max[1]; ++y)
            {
                for (x = overlap_bounds.min[0]; x <= overlap_bounds.max[0]; ++x)
                {
                    Cell *cell = world_get_cell(sim, x, y, z);
                    
                    if (cell->block_type == BLOCK_TYPE_NONE)
                        continue;

                    if (actor_bounds.max[0] <= x || actor_bounds.min[0] >= x + 1.0f)
                        continue;

                    if (actor_bounds.max[1] <= y || actor_bounds.min[1] >= y + 1.0f)
                        continue;

                    f32 block_max = z + 1.0f;

                    if (block_max <= actor_min_prev &&
                        block_max >= actor_min_next &&
                        block_max > best)
                    {
                        best = block_max;
                        found = TRUE;
                    }
                }
            }
        }

        if (found)
        {
            actor->position[2] = best + actor->box_collider.radius[2];
            actor->velocity[2] = 0.0f;
        }
        else
        {
            actor->position[2] += actor->velocity[2];
        }
    }
}

void physics_integrate(Sim *sim, Actor *actor)
{
    if (actor->movement_type == MOVEMENT_TYPE_GROUND)
    {
        actor->velocity[2] = actor->velocity[2] + sim->delta_time * sim->physics.gravity[2];  
    }

    integrate_x(sim, actor);
    integrate_y(sim, actor);
    integrate_z(sim, actor);

    glm_vec3_copy(actor->position, actor->box_collider.position);
}
