#include "sim.h"

#include "ld_data.h"

void sim_init(Sim *sim)
{
    sim->seed = 813;
    // u32 seed = (u32)time(NULL);
    
    srand(sim->seed);
    
    sim->world.cell_array = calloc(WORLD_VOLUME_IN_CELLS, sizeof(Cell));
}

void sim_close(Sim *sim)
{
    free(sim->world.cell_array);
}
