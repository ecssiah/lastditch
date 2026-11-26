// ============================================================================
// File:    src/app/mesh.h
// Layer:   app
// Purpose: CPU-side mesh generation: turning the cell table into the
//          vertices of sector meshes and loading actor models from disk.
//          Nothing here touches the GPU; the renderer uploads the results.
// ============================================================================

#ifndef APP_MESH_H
#define APP_MESH_H

#include "app/data/render/sector.h"
#include "app/data/render/vertex.h"
#include "app/world.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Function: mesh_build_sector_quads
// Purpose:  Collects the visible block faces of one sector. A face is
//           visible when its bit is set in the cell's direction mask; its
//           appearance is found by joining the cell's block type with
//           BLOCK_FACE_TYPE.
// Params:   world        - world to mesh
//           sector_index - sector to mesh, 0 <= index < WORLD_AREA_IN_SECTORS
//           quad         - receives the faces; emptied first
// Returns:  nothing
// Reads:    world.cell.block_type, world.cell.direction_mask,
//           BLOCK_FACE_TYPE
// Writes:   quad
// ----------------------------------------------------------------------------
void mesh_build_sector_quads(const World* world, s32 sector_index, SectorQuadTable* quad);

// ----------------------------------------------------------------------------
// Function: mesh_build_sector_vertices
// Purpose:  Expands the faces of a sector into packed triangle vertices.
// Params:   quad   - faces of the sector
//           vertex - receives the vertices; emptied first
// Returns:  nothing
// Reads:    quad, VOXEL_VERTEX_TABLE, VOXEL_VERTEX_INDEX_TABLE
// Writes:   vertex
// ----------------------------------------------------------------------------
void mesh_build_sector_vertices(const SectorQuadTable* quad, VoxelVertexTable* vertex);

// ----------------------------------------------------------------------------
// Function: mesh_encode_face_texture_index
// Purpose:  Converts a face type to the value stored in a voxel vertex: the
//           layer of the face's texture in the block texture array, plus
//           one. FACE_TYPE_NONE has no texture, so the face types after it
//           move down by one layer.
// Params:   face_type - face type to encode; must not be FACE_TYPE_NONE
// Returns:  The texture layer of the face type plus one.
// Reads:    arguments only
// Writes:   nothing
// ----------------------------------------------------------------------------
u32 mesh_encode_face_texture_index(FaceType face_type);

// ----------------------------------------------------------------------------
// Function: mesh_load_model
// Purpose:  Loads a triangulated Wavefront OBJ file whose faces reference a
//           position, a texture coordinate and a normal per corner.
// Params:   path   - path of the file to load
//           vertex - receives the model's vertices; emptied first
// Returns:  true when the file could be read.
// Reads:    the file at path
// Writes:   vertex
// Notes:    Lines that are not "v", "vn", "vt" or complete "f" records are
//           skipped, as are faces that reference elements the file has not
//           defined.
// ----------------------------------------------------------------------------
int mesh_load_model(const char* path, ModelVertexTable* vertex);

// ----------------------------------------------------------------------------
// Function: sector_quad_table_free
// Purpose:  Releases the storage of a quad table and resets it to empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void sector_quad_table_free(SectorQuadTable* table);

// ----------------------------------------------------------------------------
// Function: voxel_vertex_table_free
// Purpose:  Releases the storage of a voxel vertex table and resets it to
//           empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void voxel_vertex_table_free(VoxelVertexTable* table);

// ----------------------------------------------------------------------------
// Function: model_vertex_table_free
// Purpose:  Releases the storage of a model vertex table and resets it to
//           empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void model_vertex_table_free(ModelVertexTable* table);

#endif
