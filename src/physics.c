#include "physics.h"
#include "jsk_log.h"
#include "ld_data.h"
#include "world.h"

#include <math.h>

void physics_get_overlap_range(BoxCollider box_collider, Bounds *out_bounds)
{
    out_bounds->min[0] = (i32)floorf(box_collider.position[0] - box_collider.radius[0]);
    out_bounds->min[1] = (i32)floorf(box_collider.position[1] - box_collider.radius[1]);
    out_bounds->min[2] = (i32)floorf(box_collider.position[2] - box_collider.radius[2]);

    out_bounds->max[0] = (i32)ceilf(box_collider.position[0] + box_collider.radius[0]) - 1;
    out_bounds->max[1] = (i32)ceilf(box_collider.position[1] + box_collider.radius[1]) - 1;
    out_bounds->max[2] = (i32)ceilf(box_collider.position[2] + box_collider.radius[2]) - 1;

    out_bounds->min[0] = out_bounds->min[0] < 0 ? 0 : out_bounds->min[0];
    out_bounds->min[1] = out_bounds->min[1] < 0 ? 0 : out_bounds->min[1];
    out_bounds->min[2] = out_bounds->min[2] < 0 ? 0 : out_bounds->min[2];

    out_bounds->max[0] = out_bounds->max[0] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_bounds->max[0];
    out_bounds->max[1] = out_bounds->max[1] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_bounds->max[1];
    out_bounds->max[2] = out_bounds->max[2] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_bounds->max[2];
}

void get_colliding_cell_set(Sim *sim, Actor *actor)
{
    Bounds overlap_bounds;
    physics_get_overlap_range(actor->box_collider, &overlap_bounds);
    
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

void physics_resolve_collisions(Sim *sim, Actor *actor)
{
    get_colliding_cell_set(sim, actor);

    i32 index;
    for (index = 0; index < sim->physics.overlap_cell_set.count; ++index)
    {
        Cell *overlapped_cell = sim->physics.overlap_cell_set.cell_ptr_array[index];

        ivec3 cell_coordinate;
        world_cell_index_to_coordinate(overlapped_cell->cell_index, cell_coordinate);
        
        LOG_INFO("%i %i %i", cell_coordinate[0], cell_coordinate[1], cell_coordinate[2]);
    }
}
