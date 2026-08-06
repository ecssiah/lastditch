#include "actor.h"

#include "world.h"

using namespace std;

s32
Actor::find_type_index(const string& actor_type_string)
{
    for (s32 actor_type_index { 0 }; actor_type_index < ACTOR_TYPE_COUNT; ++actor_type_index)
    {
        if (actor_type_string == ACTOR_TYPE_STRING_ARRAY[actor_type_index])
        {
            return actor_type_index;
        }
    }

    return -1;
}
