#ifndef PHYSICS_H
#define PHYSICS_H

#include "justsky.h"

#include <glm/glm.hpp>
using namespace glm;

#define RISING_GRAVITY_MODIFIER 1.0f
#define FALLING_GRAVITY_MODIFIER 1.7f

struct Actor;
struct World;

struct BoxCollider
{
    b32 collision_enabled;
    
    vec3 radius;
};

void physics_update_actor(Actor *actor, World *world);

#endif
