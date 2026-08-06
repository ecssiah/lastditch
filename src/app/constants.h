#pragma once

#include "core/geometry.h"
#include "core/types.h"

constexpr u32 PRIMARY_SEED      { 1388 };

constexpr u32 WORLD_SEED        { PRIMARY_SEED + 0 };
constexpr u32 POPULATION_SEED   { PRIMARY_SEED + 1 };
constexpr u32 WORK_SEED         { PRIMARY_SEED + 2 };

constexpr f32 EPSILON { 1e-5f };
constexpr f32 EPSILON_SQUARED { 1e-10f };

constexpr f64 FRAMES_PER_SECOND { 30.0 };

constexpr f64 FIXED_FRAME_TIME_64 { 1.0  / FRAMES_PER_SECOND };
constexpr f32 FIXED_FRAME_TIME_32 { FIXED_FRAME_TIME_64 };

constexpr f64 FRAME_TIME_MAX { 0.25 };

constexpr s32 INITIAL_POPULATION_SIZE { 1 << 3 };
constexpr s32 INITIAL_POPULATION_CAPACITY { 1 << 8 };

constexpr s32 NATION_HOME_OFFSET { 80 };

constexpr f32 CELL_SIZE { 1.0f };

constexpr s32 SECTOR_SIZE_IN_CELLS_LOG2 { 5 };
constexpr s32 SECTOR_SIZE_IN_CELLS { 1 << (1 * SECTOR_SIZE_IN_CELLS_LOG2) };

constexpr s32 SECTOR_HEIGHT_IN_CELLS_LOG2 { 8 };
constexpr s32 SECTOR_HEIGHT_IN_CELLS { 1 << (1 * SECTOR_HEIGHT_IN_CELLS_LOG2) };

constexpr s32 SECTOR_AREA_IN_CELLS { 1 << (2 * SECTOR_SIZE_IN_CELLS_LOG2) };

constexpr s32 SECTOR_VOLUME_IN_CELLS { SECTOR_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS };

constexpr s32 WORLD_SIZE_IN_SECTORS_LOG2 { 3 };
constexpr s32 WORLD_SIZE_IN_SECTORS { 1 << (1 * WORLD_SIZE_IN_SECTORS_LOG2) };
constexpr s32 WORLD_AREA_IN_SECTORS { 1 << (2 * WORLD_SIZE_IN_SECTORS_LOG2) };

constexpr s32 WORLD_SIZE_IN_CELLS_LOG2 { SECTOR_SIZE_IN_CELLS_LOG2 + WORLD_SIZE_IN_SECTORS_LOG2 };
constexpr s32 WORLD_SIZE_IN_CELLS { 1 << (1 * WORLD_SIZE_IN_CELLS_LOG2) };
constexpr s32 WORLD_AREA_IN_CELLS { 1 << (2 * WORLD_SIZE_IN_CELLS_LOG2) };

constexpr s32 WORLD_VOLUME_IN_CELLS { WORLD_AREA_IN_CELLS * SECTOR_HEIGHT_IN_CELLS };

constexpr s32 WORLD_STRIDE_X { 1 };
constexpr s32 WORLD_STRIDE_Y { WORLD_SIZE_IN_CELLS };
constexpr s32 WORLD_STRIDE_Z { WORLD_AREA_IN_CELLS };

constexpr s32 WORLD_CENTER_S32 { WORLD_SIZE_IN_CELLS / 2 };
constexpr f32 WORLD_CENTER_F32 { WORLD_SIZE_IN_CELLS / 2.0f };

constexpr s32 FLOOR_SIZE_Z { 16 };
constexpr s32 FLOOR_COUNT { SECTOR_HEIGHT_IN_CELLS / FLOOR_SIZE_Z };

constexpr s32 TOWER_BORDER { 16 };
constexpr s32 TOWER_FLOOR_COUNT { 6 };
constexpr s32 TOWER_SIZE { WORLD_SIZE_IN_CELLS - 2 * TOWER_BORDER };

constexpr s32 TOWER_CENTER_HALL_SIZE { 24 };
constexpr s32 TOWER_OUTER_HALL_SIZE { 6 };

constexpr s32 TOWER_QUADRANT_SIZE { TOWER_SIZE / 2 - TOWER_OUTER_HALL_SIZE - TOWER_CENTER_HALL_SIZE / 2 };

constexpr s32 ROOF_Z { TOWER_FLOOR_COUNT * FLOOR_SIZE_Z };
constexpr s32 ROOF_FLOOR_COUNT { FLOOR_COUNT - TOWER_FLOOR_COUNT };
constexpr s32 ROOF_FLOOR_NUMBER { TOWER_FLOOR_COUNT };

constexpr s32 ROOF_CENTER_PATH_SIZE { 18 };

constexpr s32 PLATFORM_SIZE_X { 24 };
constexpr s32 PLATFORM_SIZE_Y { 16 };

constexpr s32 TEMPLE_SIZE_X { 30 };
constexpr s32 TEMPLE_SIZE_Y { 20 };

constexpr s32 TEMPLE_BORDER_OFFSET { 24 };

constexpr s32 ELEVATOR_SIZE { 16 };

constexpr b32 PLACE_ROOM_CONTENT { true };

constexpr f32 CLEAR_COLOR[4] { 0.1f, 0.2f, 0.3f, 1.0f };

constexpr s32 BLOCK_TEXTURE_SIZE { 64 };
constexpr s32 ACTOR_TEXTURE_SIZE { 128 };

constexpr s32 FACE_COUNT_PER_VOXEL { 6 };
constexpr s32 VERTEX_COUNT_PER_FACE { 4 };

constexpr std::array VERTEX_INDEX_ARRAY { 0, 1, 2, 0, 2, 3 };

constexpr IVec3 VOXEL_VERTEX_ARRAY[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE]
{
    {
        { 1, 0, 0 },
        { 1, 1, 0 },
        { 1, 1, 1 },
        { 1, 0, 1 },
    },
    {
        { 0, 1, 0 },
        { 0, 0, 0 },
        { 0, 0, 1 },
        { 0, 1, 1 },
    },
    {
        { 1, 1, 0 },
        { 0, 1, 0 },
        { 0, 1, 1 },
        { 1, 1, 1 },
    },
    {
        { 0, 0, 0 },
        { 1, 0, 0 },
        { 1, 0, 1 },
        { 0, 0, 1 },
    },
    {
        { 0, 0, 1 },
        { 1, 0, 1 },
        { 1, 1, 1 },
        { 0, 1, 1 },
    },
    {
        { 0, 1, 0 },
        { 1, 1, 0 },
        { 1, 0, 0 },
        { 0, 0, 0 },
    },
};

constexpr Vec3
VOXEL_UV_PROJECTION_ARRAY[2 * FACE_COUNT_PER_VOXEL]
{
    { +0, +1, +0 }, { +0, +0, +1 },
    { +0, -1, +0 }, { +0, +0, +1 },
    { -1, +0, +0 }, { +0, +0, +1 },
    { +1, +0, +0 }, { +0, +0, +1 },
    { +1, +0, +0 }, { +0, +1, +0 },
    { +1, +0, +0 }, { +0, -1, +0 },
};
