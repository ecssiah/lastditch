#pragma once

#include "core/geometry.h"
#include "core/macros.h"
#include "core/types.h"

#define FOR_LIST_NATION_TYPE(DO)                                            \
    DO(Wolf)                                                                    \
    DO(Eagle)                                                                   \
    DO(Lion)                                                                    \
    DO(Bear)                                                                    \

enum class NationType : u8
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_VARIANTS)
};
    
constexpr s32 NATION_TYPE_COUNT
{
    FOR_LIST_NATION_TYPE(DEFINE_ENUM_COUNT)
};

extern const char* NATION_TYPE_STRING_ARRAY[NATION_TYPE_COUNT];

class Nation
{
public:
    static s32 get_type_index(const std::string& nation_type_string);

    NationType nation_type {};
    IVec3 home_coordinate {};
};

