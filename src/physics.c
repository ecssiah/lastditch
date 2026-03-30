#include "physics.h"
#include "ld_data.h"
#include "world.h"

#include <math.h>

void physics_get_overlap_range(BoxCollider box_collider, vec3 world_position, ivec3 out_min, ivec3 out_max)
{
    out_min[0] = (i32)floorf(world_position[0] - box_collider.radius[0]);
    out_min[1] = (i32)floorf(world_position[1] - box_collider.radius[1]);
    out_min[2] = (i32)floorf(world_position[2] - box_collider.radius[2]);

    out_max[0] = (i32)ceilf(world_position[0] + box_collider.radius[0]) - 1;
    out_max[1] = (i32)ceilf(world_position[1] + box_collider.radius[1]) - 1;
    out_max[2] = (i32)ceilf(world_position[2] + box_collider.radius[2]) - 1;

    out_min[0] = out_min[0] < 0 ? 0 : out_min[0];
    out_min[1] = out_min[1] < 0 ? 0 : out_min[1];
    out_min[2] = out_min[2] < 0 ? 0 : out_min[2];

    out_max[0] = out_max[0] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_max[0];
    out_max[1] = out_max[1] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_max[1];
    out_max[2] = out_max[2] >= WORLD_SIZE_IN_CELLS ? WORLD_SIZE_IN_CELLS - 1: out_max[2];
}

void physics_resolve_collisions(Sim *sim, Actor *actor)
{
    ivec3 overlap_min, overlap_max;

    physics_get_overlap_range(actor->box_collider, actor->world_position, overlap_min, overlap_max);

    b32 collision = FALSE;
    
    i32 x, y, z;
    for (z = overlap_min[2]; z <= overlap_max[2]; ++z)
    {
        for (y = overlap_min[1]; y <= overlap_max[1]; ++y)
        {
            for (x = overlap_min[0]; x <= overlap_max[0]; ++x)
            {
                if (world_is_solid(sim, x, y, z))
                {
                    collision = TRUE;
                }
            }
        }
    }

    printf("%s\n", collision == TRUE ? "TRUE" : "FALSE");
}
