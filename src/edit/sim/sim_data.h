#ifndef SIM_DATA_H
#define SIM_DATA_H

#include "jsk.h"

#include "core/core_data.h"

#define EDIT_SIZE_IN_CELLS_LOG2 4
#define EDIT_SIZE_IN_CELLS (1 << (1u * EDIT_SIZE_IN_CELLS_LOG2))
#define EDIT_VOLUME_IN_CELLS (1 << (3u * EDIT_SIZE_IN_CELLS_LOG2))

typedef struct Edit Edit;
struct Edit
{
    b32 active;
    
    f64 previous_time;
    f64 current_time;

    f32 delta_time;

    ActionQueue action_queue;

    Cell *cell_array;
};

#endif
