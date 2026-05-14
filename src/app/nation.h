#pragma once

#include "core/geometry.h"
#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_NATION_TYPE(DO)                                                \
    DO(Wolf)                                                                    \
    DO(Eagle)                                                                   \
    DO(Lion)                                                                    \
    DO(Bear)                                                                    \

enum class NationType : u8
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_VARIANTS)
};
    
constexpr i32 nation_type_count = FOR_LIST_NATION_TYPE(DEFINE_ENUM_COUNT);

extern const char* nation_type_string_array[nation_type_count];

struct Nation
{
    NationType nation_type;

    IVec3 home_coordinate;
};

i32 nation_type_index_from_string(const std::string& nation_type_string);
