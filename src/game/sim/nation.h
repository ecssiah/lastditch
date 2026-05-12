#pragma once

#include <glm/glm.hpp>

#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_NATION_TYPE(DO)                                               \
  DO(wolf)                                                                     \
  DO(eagle)                                                                    \
  DO(lion)                                                                     \
  DO(bear)

enum class NationType
{
    FOR_LIST_NATION_TYPE(DEFINE_LIST_ENUMERATION)
};
    
constexpr i32 NATION_TYPE_COUNT = FOR_LIST_NATION_TYPE(DEFINE_ENUM_COUNT);

extern const char* NATION_TYPE_STRING[NATION_TYPE_COUNT];

struct Nation
{
    NationType nation_type;

    glm::ivec3 home_coordinate;
};

i32 nation_type_index_from_string(const char* nation_type_string);
