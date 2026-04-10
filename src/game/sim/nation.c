#include "game/sim/nation.h"

#include <string.h>

#include "game/sim/world.h"

const char *NATION_TYPE_STRING[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_STRING )
};

i32 nation_type_index_from_string(const char *nation_type_string)
{
    for (u32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        if (strcmp(nation_type_string, NATION_TYPE_STRING[nation_type_index]) == 0)
        {
            return nation_type_index;
        }
    }

    return -1;
}

void nation_init(Nation nation_array[4])
{
    const f32 nation_offset = 76.0f;
    
    Nation *wolf_nation = &nation_array[NATION_TYPE_WOLF];
    
    wolf_nation->nation_type = NATION_TYPE_WOLF;

    wolf_nation->home_coordinate[0] = WORLD_CENTER_F32 + nation_offset;
    wolf_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    wolf_nation->home_coordinate[2] = ROOF_Z + 3.0f;

    Nation *eagle_nation = &nation_array[NATION_TYPE_EAGLE];
    
    eagle_nation->nation_type = NATION_TYPE_EAGLE;

    eagle_nation->home_coordinate[0] = WORLD_CENTER_F32 - nation_offset;
    eagle_nation->home_coordinate[1] = WORLD_CENTER_F32 + 0.0f;
    eagle_nation->home_coordinate[2] = ROOF_Z + 3.0f;

    Nation *bear_nation = &nation_array[NATION_TYPE_BEAR];
    
    bear_nation->nation_type = NATION_TYPE_BEAR;

    bear_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    bear_nation->home_coordinate[1] = WORLD_CENTER_F32 + nation_offset;
    bear_nation->home_coordinate[2] = ROOF_Z + 1.0f;
    
    Nation *lion_nation = &nation_array[NATION_TYPE_LION];

    lion_nation->nation_type = NATION_TYPE_LION;

    lion_nation->home_coordinate[0] = WORLD_CENTER_F32 + 0.0f;
    lion_nation->home_coordinate[1] = WORLD_CENTER_F32 - nation_offset;
    lion_nation->home_coordinate[2] = ROOF_Z + 3.0f;
}
