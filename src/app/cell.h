#pragma once

#include <stdexcept>

#include "constants.h"
#include "direction.h"
#include "core/types.h"

enum class BlockType : u8
{
    None,
    Compass,
    WolfFlag,
    EagleFlag,
    LionFlag,
    BearFlag,
    WolfStone,
    EagleStone,
    LionStone,
    BearStone,
    WolfSymbol,
    EagleSymbol,
    LionSymbol,
    BearSymbol,
    Carved1,
    Carved2,
    Carved3,
    Carved4,
    Caution1,
    Caution2,
    Caution3,
    Caution4,
    Engraved1,
    Engraved2,
    Engraved3,
    Engraved4,
    Metal1,
    Metal2,
    Metal3,
    Metal4,
    Metal5,
    Ornate1,
    Ornate2,
    Ornate3,
    Ornate4,
    Panel1,
    Panel2,
    Panel3,
    Panel4,
    Server1,
    Server2,
    Server3,
    Server4,
    Server5,
    Server6,
    Server7,
    Smooth1,
    Smooth2,
    Smooth3,
    Smooth4,
    Vent1,
    Vent2,
    Vent3,
    Vent4,
    COUNT,
};

constexpr std::string_view
get_block_type_string(const BlockType block_type)
{
    switch (block_type)
    {
        case BlockType::None:                   return "None";
        case BlockType::Compass:                return "Compass";
        case BlockType::WolfFlag:               return "WolfFlag";
        case BlockType::EagleFlag:              return "EagleFlag";
        case BlockType::LionFlag:               return "LionFlag";
        case BlockType::BearFlag:               return "BearFlag";
        case BlockType::WolfStone:              return "WolfStone";
        case BlockType::EagleStone:             return "EagleStone";
        case BlockType::LionStone:              return "LionStone";
        case BlockType::BearStone:              return "BearStone";
        case BlockType::WolfSymbol:             return "WolfSymbol";
        case BlockType::EagleSymbol:            return "EagleSymbol";
        case BlockType::LionSymbol:             return "LionSymbol";
        case BlockType::BearSymbol:             return "BearSymbol";
        case BlockType::Carved1:                return "Carved1";
        case BlockType::Carved2:                return "Carved2";
        case BlockType::Carved3:                return "Carved3";
        case BlockType::Carved4:                return "Carved4";
        case BlockType::Caution1:               return "Caution1";
        case BlockType::Caution2:               return "Caution2";
        case BlockType::Caution3:               return "Caution3";
        case BlockType::Caution4:               return "Caution4";
        case BlockType::Engraved1:              return "Engraved1";
        case BlockType::Engraved2:              return "Engraved2";
        case BlockType::Engraved3:              return "Engraved3";
        case BlockType::Engraved4:              return "Engraved4";
        case BlockType::Metal1:                 return "Metal1";
        case BlockType::Metal2:                 return "Metal2";
        case BlockType::Metal3:                 return "Metal3";
        case BlockType::Metal4:                 return "Metal4";
        case BlockType::Metal5:                 return "Metal5";
        case BlockType::Ornate1:                return "Ornate1";
        case BlockType::Ornate2:                return "Ornate2";
        case BlockType::Ornate3:                return "Ornate3";
        case BlockType::Ornate4:                return "Ornate4";
        case BlockType::Panel1:                 return "Panel1";
        case BlockType::Panel2:                 return "Panel2";
        case BlockType::Panel3:                 return "Panel3";
        case BlockType::Panel4:                 return "Panel4";
        case BlockType::Server1:                return "Server1";
        case BlockType::Server2:                return "Server2";
        case BlockType::Server3:                return "Server3";
        case BlockType::Server4:                return "Server4";
        case BlockType::Server5:                return "Server5";
        case BlockType::Server6:                return "Server6";
        case BlockType::Server7:                return "Server7";
        case BlockType::Smooth1:                return "Smooth1";
        case BlockType::Smooth2:                return "Smooth2";
        case BlockType::Smooth3:                return "Smooth3";
        case BlockType::Smooth4:                return "Smooth4";
        case BlockType::Vent1:                  return "Vent1";
        case BlockType::Vent2:                  return "Vent2";
        case BlockType::Vent3:                  return "Vent3";
        case BlockType::Vent4:                  return "Vent4";
        default:                                throw std::invalid_argument("invalid block type");
    }
}

constexpr s32 BLOCK_TYPE_COUNT { static_cast<s32>(BlockType::COUNT) };

enum class FaceType : u8
{
    None,
    East,
    West,
    North,
    South,
    Up,
    Down,
    WolfImage,
    EagleImage,
    LionImage,
    BearImage,
    WolfText,
    EagleText,
    LionText,
    BearText,
    WolfStone,
    EagleStone,
    LionStone,
    BearStone,
    Carved1,
    Carved2,
    Carved3,
    Carved4,
    Caution1,
    Caution2,
    Caution3,
    Caution4,
    Engraved1,
    Engraved2,
    Engraved3,
    Engraved4,
    Metal1,
    Metal2,
    Metal3,
    Metal4,
    Metal5,
    Ornate1,
    Ornate2,
    Ornate3,
    Ornate4,
    Panel1,
    Panel2,
    Panel3,
    Panel4,
    Server1,
    Server2,
    Server3,
    Server4,
    Server5,
    Server6,
    Server7,
    Smooth1,
    Smooth2,
    Smooth3,
    Smooth4,
    Vent1,
    Vent2,
    Vent3,
    Vent4,
    COUNT,
};

constexpr s32 FACE_TYPE_COUNT { static_cast<s32>(FaceType::COUNT) };

inline static constexpr
std::array<std::array<FaceType, FACE_COUNT_PER_VOXEL>, BLOCK_TYPE_COUNT> BLOCK_TYPE_TO_FACE_TYPE_ARRAY
{{
    // None
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Compass
    {{
        FaceType::East,
        FaceType::West,
        FaceType::North,
        FaceType::South,
        FaceType::Up,
        FaceType::Down,
    }},
    // WolfFlag
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // EagleFlag
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // LionFlag
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // BearFlag
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // WolfStone
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // EagleStone
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // LionStone
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // BearStone
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // WolfSymbol
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // EagleSymbol
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // LionSymbol
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // BearSymbol
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Carved1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Carved2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Carved3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Carved4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Caution1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Caution2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Caution3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Caution4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Engraved1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Engraved2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Engraved3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Engraved4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Metal1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Metal2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Metal3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Metal4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Metal5
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Ornate1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Ornate2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Ornate3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Ornate4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Panel1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Panel2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Panel3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Panel4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server5
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server6
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Server7
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Smooth1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Smooth2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Smooth3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Smooth4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Vent1
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Vent2
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Vent3
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
    // Vent 4
    {{
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    }},
}};

struct Cell
{
    s32 cell_index          { -1 };
    u8 direction_mask       { 0 };
    BlockType block_type    { BlockType::None };

    std::array<FaceType, FACE_COUNT_PER_VOXEL> face_type_array {
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
        FaceType::None,
    };
};
