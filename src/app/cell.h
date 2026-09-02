#pragma once

#include "block_data.generated.h"
#include "constants.h"
#include "core/types.h"

struct Cell
{
    s32 cell_index          { -1 };
    u8 direction_mask       { 0 };
    BlockType block_type    { BlockType::none };

    std::array<FaceType, FACE_COUNT_PER_VOXEL> face_type_array {
        FaceType::none,
        FaceType::none,
        FaceType::none,
        FaceType::none,
        FaceType::none,
        FaceType::none,
    };
};
