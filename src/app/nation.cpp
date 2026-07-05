#include "app/nation.h"

#include "app/world.h"

using namespace std;

const char* nation_type_string_array[NATION_TYPE_COUNT] =
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_STRINGS)
};

s32 
nation_type_index_from_string(const string& nation_type_string)
{
    for (s32 nation_type_index = 0; nation_type_index < NATION_TYPE_COUNT; ++nation_type_index)
    {
        if (nation_type_string == nation_type_string_array[nation_type_index])
        {
            return nation_type_index;
        }
    }
    
    return -1;
}
