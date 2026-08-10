#pragma once

#include "core/types.h"

enum class BlockType : u8
{
    None,
    CardinalEast,
    CardinalWest,
    CardinalNorth,
    CardinalSouth,
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

constexpr s32 BLOCK_TYPE_COUNT { static_cast<s32>(BlockType::COUNT) };

constexpr std::string
get_block_type_string(const BlockType block_type)
{
    switch (block_type)
    {
        case BlockType::None:               return "None";
        case BlockType::CardinalEast:       return "CardinalEast";
        case BlockType::CardinalWest:       return "CardinalWest";
        case BlockType::CardinalNorth:      return "CardinalNorth";
        case BlockType::CardinalSouth:      return "CardinalSouth";
        case BlockType::WolfFlag:           return "WolfFlag";
        case BlockType::EagleFlag:          return "EagleFlag";
        case BlockType::LionFlag:           return "LionFlag";
        case BlockType::BearFlag:           return "BearFlag";
        case BlockType::WolfStone:          return "WolfStone";
        case BlockType::EagleStone:         return "EagleStone";
        case BlockType::LionStone:          return "LionStone";
        case BlockType::BearStone:          return "BearStone";
        case BlockType::WolfSymbol:         return "WolfSymbol";
        case BlockType::EagleSymbol:        return "EagleSymbol";
        case BlockType::LionSymbol:         return "LionSymbol";
        case BlockType::BearSymbol:         return "BearSymbol";
        case BlockType::Carved1:            return "Carved1";
        case BlockType::Carved2:            return "Carved2";
        case BlockType::Carved3:            return "Carved3";
        case BlockType::Carved4:            return "Carved4";
        case BlockType::Caution1:           return "Caution1";
        case BlockType::Caution2:           return "Caution2";
        case BlockType::Caution3:           return "Caution3";
        case BlockType::Caution4:           return "Caution4";
        case BlockType::Engraved1:          return "Engraved1";
        case BlockType::Engraved2:          return "Engraved2";
        case BlockType::Engraved3:          return "Engraved3";
        case BlockType::Engraved4:          return "Engraved4";
        case BlockType::Metal1:             return "Metal1";
        case BlockType::Metal2:             return "Metal2";
        case BlockType::Metal3:             return "Metal3";
        case BlockType::Metal4:             return "Metal4";
        case BlockType::Metal5:             return "Metal5";
        case BlockType::Ornate1:            return "Ornate1";
        case BlockType::Ornate2:            return "Ornate2";
        case BlockType::Ornate3:            return "Ornate3";
        case BlockType::Ornate4:            return "Ornate4";
        case BlockType::Panel1:             return "Panel1";
        case BlockType::Panel2:             return "Panel2";
        case BlockType::Panel3:             return "Panel3";
        case BlockType::Panel4:             return "Panel4";
        case BlockType::Server1:            return "Server1";
        case BlockType::Server2:            return "Server2";
        case BlockType::Server3:            return "Server3";
        case BlockType::Server4:            return "Server4";
        case BlockType::Server5:            return "Server5";
        case BlockType::Server6:            return "Server6";
        case BlockType::Server7:            return "Server7";
        case BlockType::Smooth1:            return "Smooth1";
        case BlockType::Smooth2:            return "Smooth2";
        case BlockType::Smooth3:            return "Smooth3";
        case BlockType::Smooth4:            return "Smooth4";
        case BlockType::Vent1:              return "Vent1";
        case BlockType::Vent2:              return "Vent2";
        case BlockType::Vent3:              return "Vent3";
        case BlockType::Vent4:              return "Vent4";
        default:                            throw std::invalid_argument("invalid block type");
    }
}

struct Cell
{
    s32 cell_index          { -1 };
    BlockType block_type    { BlockType::None };
    u8 direction_mask       { 0 };
};
