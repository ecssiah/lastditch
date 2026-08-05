#pragma once

#include "core/geometry.h"
#include "core/types.h"

enum class NationType : u8
{
    Wolf,
    Eagle,
    Lion,
    Bear,
    COUNT,
};

constexpr s32 NATION_TYPE_COUNT { static_cast<std::size_t>(NationType::COUNT) };

constexpr std::string_view
get_nation_type_string(const NationType nation_type)
{
    switch (nation_type)
    {
        case NationType::Wolf:      return "Wolf";
        case NationType::Eagle:     return "Eagle";
        case NationType::Lion:      return "Lion";
        case NationType::Bear:      return "Bear";
        default:                    throw std::invalid_argument("Invalid Nation Type");
    }
}

constexpr s32
find_nation_type_index(const std::string_view nation_type_string)
{
    for (s32 index = 0; index < NATION_TYPE_COUNT; index++)
    {
        const NationType nation_type { static_cast<NationType>(index) };

        if (nation_type_string == get_nation_type_string(nation_type))
        {
            return index;
        }
    }

    return -1;
}

class Nation
{
public:
    NationType nation_type {};
    IVec3 home_coordinate {};
};

