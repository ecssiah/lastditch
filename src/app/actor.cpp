#include "app/actor.h"

#include "app/world.h"

using namespace std;

const char* actor_type_string_array[ACTOR_TYPE_COUNT] =
{
    FOR_LIST_ACTOR_TYPE(DEFINE_ENUM_STRINGS)
};

s32 
Actor::get_type_index(const string& actor_type_string)
{
    for (s32 actor_type_index = 0; actor_type_index < ACTOR_TYPE_COUNT; ++actor_type_index)
    {
        if (actor_type_string == actor_type_string_array[actor_type_index])
        {
            return actor_type_index;
        }
    }

    return -1;
}
