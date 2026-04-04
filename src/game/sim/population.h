#ifndef POPULATION_H
#define POPULATION_H

#include "game/sim/sim_data.h"

i32 population_actor_type_index_from_string(const char *actor_type_string);

void population_init(Sim *sim);
void population_update(Sim *sim);
void population_close(Sim *sim);

#endif
