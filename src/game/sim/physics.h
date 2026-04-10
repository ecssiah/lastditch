#ifndef PHYSICS_H
#define PHYSICS_H

#include "jsk.h"

#include <cglm/cglm.h>

#define RISING_GRAVITY_MODIFIER 1.0f
#define FALLING_GRAVITY_MODIFIER 1.7f

typedef struct Actor Actor;
typedef struct World World;

typedef struct BoxCollider BoxCollider;
struct BoxCollider
{
    b32 collision_enabled;
    
    vec3 radius;
};

void physics_update_actor(Actor *actor, World *world);

#endif
