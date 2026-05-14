#include "app/nation.h"

#include "app/world.h"

const char* NATION_TYPE_STRING[nation_type_count] =
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_STRINGS)
};

i32 
nation_type_index_from_string(const std::string& nation_type_string)
{
    for (i32 nation_type_index = 0; nation_type_index < nation_type_count; ++nation_type_index)
    {
        if (nation_type_string == NATION_TYPE_STRING[nation_type_index])
        {
            return nation_type_index;
        }
    }

    return -1;
}
