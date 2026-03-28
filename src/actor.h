#ifndef ACTOR_H
#define ACTOR_H

#include "ld_data.h"

void actor_get_forward(Actor *actor, vec3 out_forward);
void actor_get_right(Actor *actor, vec3 out_right);
void actor_get_up(Actor *actor, vec3 out_up);

#endif
