#ifndef NATION_H
#define NATION_H

#include "justsky.h"

#include <cglm/cglm.h>

#define FOR_LIST_NATION_TYPE(DO)                \
    DO(NATION_TYPE_WOLF)                        \
    DO(NATION_TYPE_EAGLE)                       \
    DO(NATION_TYPE_LION)                        \
    DO(NATION_TYPE_BEAR)                        \

typedef enum NationType NationType;
enum NationType
{
    FOR_LIST_NATION_TYPE(DEFINE_LIST_ENUMERATION)
    NATION_TYPE_COUNT
};

extern const char *NATION_TYPE_STRING[NATION_TYPE_COUNT];

typedef struct Nation Nation;
struct Nation
{
    NationType nation_type;

    ivec3 home_coordinate;
};

i32 nation_type_index_from_string(const char *nation_type_string);

void nation_init(Nation nation_array[4]);

#endif 
