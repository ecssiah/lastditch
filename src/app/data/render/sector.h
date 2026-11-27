// ============================================================================
// File:    src/app/data/render/sector.h
// Layer:   app
// Purpose: Staging tables used to turn the cell table into sector meshes:
//          the visible faces of a sector, the vertices generated from them
//          and the vertices of a loaded model. These tables are scratch
//          space; they are refilled for each sector or model and hold no
//          long-lived state.
// ============================================================================

#ifndef APP_DATA_RENDER_SECTOR_H
#define APP_DATA_RENDER_SECTOR_H

#include "app/data/block_reference.h"

#include "app/data/render/vertex.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Constants: sector mesh geometry
// Purpose:   How block faces become triangles.
// ----------------------------------------------------------------------------
// Vertices emitted per visible face: two triangles.
#define SECTOR_VERTEX_COUNT_PER_QUAD 6

// ----------------------------------------------------------------------------
// Table:    SectorQuadTable
// Purpose:  The visible block faces of one sector: one row per face that
//           borders an empty cell and has a face type to show.
// Key:      (local_x, local_y, local_z, direction)
// Order:    by cell (z, then y, then x), then by Direction
// Lifetime: staging; emptied and refilled by mesh_build_sector_quads for
//           each sector, released by sector_quad_table_free
// ----------------------------------------------------------------------------
typedef struct SectorQuadTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Cell X relative to the sector origin, 0 <= x < SECTOR_SIZE_IN_CELLS.
    u8 *local_x;
    // Cell Y relative to the sector origin, 0 <= y < SECTOR_SIZE_IN_CELLS.
    u8 *local_y;
    // Cell Z, 0 <= z < SECTOR_HEIGHT_IN_CELLS.
    u8 *local_z;
    // Direction the face points in.
    u8 *direction;
    // [FK -> FACE_TYPE_NAME] Appearance of the face; never FACE_TYPE_NONE.
    FaceType *face_type;
} SectorQuadTable;

// ----------------------------------------------------------------------------
// Table:    VoxelVertexTable
// Purpose:  The vertices of one sector mesh, in the layout uploaded to the
//           GPU: SECTOR_VERTEX_COUNT_PER_QUAD consecutive rows per quad.
// Key:      row number
// Order:    the order of the quads they were generated from
// Lifetime: staging; emptied and refilled by mesh_build_sector_vertices for
//           each sector, released by voxel_vertex_table_free
// ----------------------------------------------------------------------------
typedef struct VoxelVertexTable
{
    // Number of vertices stored.
    s32 row_count;
    // Number of vertices the column can hold.
    s32 row_capacity;
    // Packed vertices, in GPU layout.
    VoxelVertex *vertex;
} VoxelVertexTable;

// ----------------------------------------------------------------------------
// Table:    ModelVertexTable
// Purpose:  The vertices of one loaded model, in the layout uploaded to the
//           GPU: three consecutive rows per triangle.
// Key:      row number
// Order:    the order of the faces in the model file
// Lifetime: staging; emptied and refilled by mesh_load_model, released by
//           model_vertex_table_free
// ----------------------------------------------------------------------------
typedef struct ModelVertexTable
{
    // Number of vertices stored.
    s32 row_count;
    // Number of vertices the column can hold.
    s32 row_capacity;
    // Vertices, in GPU layout.
    ModelVertex *vertex;
} ModelVertexTable;

#endif
