#include "game/sim/nation.h"

#include "game/sim/world.h"

const char* NATION_TYPE_STRING[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_STRINGS)
};

i32 
nation_type_index_from_string(const std::string& nation_type_string)
{
    for (i32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        if (nation_type_string == NATION_TYPE_STRING[nation_type_index])
        {
            return nation_type_index;
        }
    }

    return -1;
}
