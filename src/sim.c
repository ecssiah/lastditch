#include "sim.h"

#include "ld_data.h"

void sim_init(Sim *sim)
{
    sim->active = True;
    
    sim->seed = 813;
    // u32 seed = (u32)time(NULL);
    
    srand(sim->seed);
    
    sim->cell_array = calloc(WORLD_VOLUME_IN_CELLS, sizeof(Cell));
}

void sim_update(Sim *sim)
{

}

void sim_close(Sim *sim)
{
    free(sim->cell_array);
}
