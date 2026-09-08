#pragma once

#include "core/geometry.h"
#include "core/types.h"

enum class FaceType : u16;
enum class Direction : u8;

struct SectorQuad
{
    IVec3 local_coordinate {};
    Direction direction {};
    FaceType face_type {};
};

struct SectorMesh
{
    s32 sector_index {};

    std::vector<SectorQuad> sector_quad_vector {};
};
