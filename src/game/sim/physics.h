#ifndef PHYSICS_H
#define PHYSICS_H

#include "game/sim/population.h"
#include "game/sim/world.h"

void physics_integrate(World *world, Actor *actor);

#endif
