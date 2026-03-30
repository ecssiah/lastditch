#ifndef PHYSICS_H
#define PHYSICS_H

#include "ld_data.h"

void physics_get_overlap_range(BoxCollider box_collider, Bounds *out_bounds);

void physics_resolve_collisions(Sim *sim, Actor *actor);

#endif
