#ifndef PHYSICS_H
#define PHYSICS_H

#include "ld_data.h"

void physics_get_overlap_range(BoxCollider box_collider, vec3 world_position, ivec3 out_min, ivec3 out_max);

void physics_resolve_collisions(Sim *sim, Actor *actor);

#endif
