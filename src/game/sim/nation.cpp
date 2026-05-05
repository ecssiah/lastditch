#include "game/sim/nation.h"

#include <string.h>

#include "game/sim/world.h"

const char *NATION_TYPE_STRING[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE( DEFINE_LIST_STRING )
};

i32 nation_type_index_from_string(const char *nation_type_string)
{
    for (i32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        if (strcmp(nation_type_string, NATION_TYPE_STRING[nation_type_index]) == 0)
        {
            return nation_type_index;
        }
    }

    return -1;
}
