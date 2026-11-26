// ============================================================================
// File:    src/app/mesh.c
// Layer:   app
// Purpose: Implementation of CPU-side mesh generation declared in
//          app/mesh.h.
// ============================================================================

#include "app/mesh.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>

#include <SDL3/SDL_iostream.h>

#include "app/data/constants.h"
#include "app/data/direction.h"
#include "core/table.h"

// ----------------------------------------------------------------------------
// Reference: VOXEL_VERTEX_TABLE
// Purpose:   Corner offsets of each face of a unit cube, relative to the
//            cell's lower corner. The corners of a face are listed
//            counter-clockwise as seen from outside the cube.
// Key:       Direction, then corner number, then Axis
// Order:     faces in Direction order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const s32 VOXEL_VERTEX_TABLE[FACE_COUNT_PER_VOXEL][VERTEX_COUNT_PER_FACE][COORDINATES_PER_VERTEX] =
{
    { { 1, 0, 0 }, { 1, 1, 0 }, { 1, 1, 1 }, { 1, 0, 1 } },
    { { 0, 1, 0 }, { 0, 0, 0 }, { 0, 0, 1 }, { 0, 1, 1 } },
    { { 1, 1, 0 }, { 0, 1, 0 }, { 0, 1, 1 }, { 1, 1, 1 } },
    { { 0, 0, 0 }, { 1, 0, 0 }, { 1, 0, 1 }, { 0, 0, 1 } },
    { { 0, 0, 1 }, { 1, 0, 1 }, { 1, 1, 1 }, { 0, 1, 1 } },
    { { 0, 1, 0 }, { 1, 1, 0 }, { 1, 0, 0 }, { 0, 0, 0 } }
};

// ----------------------------------------------------------------------------
// Reference: VOXEL_VERTEX_INDEX_TABLE
// Purpose:   The corners of a face, in the order that draws it as two
//            counter-clockwise triangles.
// Key:       vertex number within the quad
// Order:     first triangle, then second triangle
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const s32 VOXEL_VERTEX_INDEX_TABLE[SECTOR_VERTEX_COUNT_PER_QUAD] = { 0, 1, 2, 0, 2, 3 };

// ----------------------------------------------------------------------------
// Function: sector_quad_table_reserve
// Purpose:  Ensures every column of a quad table can hold a given number
//           of rows.
// Params:   table        - table to grow
//           row_required - number of rows that must fit
// Returns:  nothing
// Reads:    table.row_capacity
// Writes:   table: column storage, row_capacity
// ----------------------------------------------------------------------------
static void
sector_quad_table_reserve(SectorQuadTable* table, const s32 row_required)
{
    const s32 capacity = table_grow_capacity(table->row_capacity, row_required);

    if (capacity == table->row_capacity)
    {
        return;
    }

    table->local_x = (u8*)column_resize(table->local_x, sizeof(u8), capacity);
    table->local_y = (u8*)column_resize(table->local_y, sizeof(u8), capacity);
    table->local_z = (u8*)column_resize(table->local_z, sizeof(u8), capacity);
    table->direction = (u8*)column_resize(table->direction, sizeof(u8), capacity);
    table->face_type = (FaceType*)column_resize(table->face_type, sizeof(FaceType), capacity);

    table->row_capacity = capacity;
}

// Documented in src/app/mesh.h.
void
mesh_build_sector_quads(const World* world, const s32 sector_index, SectorQuadTable* quad)
{
    const IVec2 sector_coordinate = world_sector_index_to_coordinate(sector_index);
    const s32 origin_x = sector_coordinate.x * SECTOR_SIZE_IN_CELLS;
    const s32 origin_y = sector_coordinate.y * SECTOR_SIZE_IN_CELLS;
    s32 x;
    s32 y;
    s32 z;

    quad->row_count = 0;

    for (z = 0; z < SECTOR_HEIGHT_IN_CELLS; ++z)
    {
        for (y = origin_y; y < origin_y + SECTOR_SIZE_IN_CELLS; ++y)
        {
            for (x = origin_x; x < origin_x + SECTOR_SIZE_IN_CELLS; ++x)
            {
                s32 cell_index;
                BlockType block_type;
                u8 mask;

                if (!world_cell_coordinate_is_valid(x, y, z))
                {
                    continue;
                }

                cell_index = world_cell_coordinate_to_index(x, y, z);
                block_type = world->cell.block_type[cell_index];

                if (block_type == BLOCK_TYPE_NONE)
                {
                    continue;
                }

                mask = world->cell.direction_mask[cell_index];

                while (mask != 0)
                {
                    const Direction direction = direction_from_mask(mask);
                    const FaceType face_type = BLOCK_FACE_TYPE[block_type][direction];

                    if (face_type != FACE_TYPE_NONE)
                    {
                        const s32 row = quad->row_count;

                        sector_quad_table_reserve(quad, row + 1);

                        quad->local_x[row] = (u8)(x - origin_x);
                        quad->local_y[row] = (u8)(y - origin_y);
                        quad->local_z[row] = (u8)z;
                        quad->direction[row] = (u8)direction;
                        quad->face_type[row] = face_type;

                        quad->row_count = row + 1;
                    }

                    // Clear the lowest set bit: the direction just handled.
                    mask &= (u8)(mask - 1);
                }
            }
        }
    }
}

// Documented in src/app/mesh.h.
u32
mesh_encode_face_texture_index(const FaceType face_type)
{
    const u32 face_type_index = (u32)face_type;
    const u32 none_index = (u32)FACE_TYPE_NONE;
    const u32 texture_layer = face_type_index < none_index ? face_type_index : face_type_index - 1;

    assert(face_type != FACE_TYPE_NONE);

    // The sector shader expects a one-based value and subtracts one before
    // sampling.
    return texture_layer + 1;
}

// Documented in src/app/mesh.h.
void
mesh_build_sector_vertices(const SectorQuadTable* quad, VoxelVertexTable* vertex)
{
    const s32 vertex_count = quad->row_count * SECTOR_VERTEX_COUNT_PER_QUAD;
    const s32 capacity = table_grow_capacity(vertex->row_capacity, vertex_count);
    s32 row;
    s32 index;

    if (capacity != vertex->row_capacity)
    {
        vertex->vertex = (VoxelVertex*)column_resize(vertex->vertex, sizeof(VoxelVertex), capacity);
        vertex->row_capacity = capacity;
    }

    vertex->row_count = 0;

    for (row = 0; row < quad->row_count; ++row)
    {
        const u32 direction = quad->direction[row];
        const u32 face_texture_index = mesh_encode_face_texture_index(quad->face_type[row]);

        for (index = 0; index < SECTOR_VERTEX_COUNT_PER_QUAD; ++index)
        {
            const s32* corner = VOXEL_VERTEX_TABLE[direction][VOXEL_VERTEX_INDEX_TABLE[index]];
            const u32 position_x = (u32)(quad->local_x[row] + corner[0]);
            const u32 position_y = (u32)(quad->local_y[row] + corner[1]);
            const u32 position_z = (u32)(quad->local_z[row] + corner[2]);
            VoxelVertex* voxel_vertex = &vertex->vertex[vertex->row_count++];

            voxel_vertex->vertex =
                (position_x & 63u) |
                ((position_y & 63u) << 6u) |
                ((position_z & 255u) << 12u);

            voxel_vertex->face_type =
                (face_texture_index & 255u) |
                ((direction & 7u) << 8u);
        }
    }
}

// ----------------------------------------------------------------------------
// Function: mesh_append_f32
// Purpose:  Appends values to a growing array of f32 tuples, used while
//           parsing a model file.
// Params:   column       - storage of the array; may be reallocated
//           count        - number of tuples stored; incremented
//           capacity     - number of tuples the storage can hold; updated
//           values       - the tuple to append
//           value_count  - number of f32 values per tuple
// Returns:  nothing
// Reads:    values
// Writes:   column, count, capacity
// ----------------------------------------------------------------------------
static void
mesh_append_f32(f32** column, s32* count, s32* capacity, const f32* values, const s32 value_count)
{
    const s32 grown = table_grow_capacity(*capacity, *count + 1);
    s32 index;

    if (grown != *capacity)
    {
        *column = (f32*)column_resize(*column, sizeof(f32) * (size_t)value_count, grown);
        *capacity = grown;
    }

    for (index = 0; index < value_count; ++index)
    {
        (*column)[*count * value_count + index] = values[index];
    }

    *count += 1;
}

// Documented in src/app/mesh.h.
int
mesh_load_model(const char* path, ModelVertexTable* vertex)
{
    size_t file_size = 0;
    char* file_data = (char*)SDL_LoadFile(path, &file_size);
    char* line;
    f32* position_array = NULL;
    f32* normal_array = NULL;
    f32* uv_array = NULL;
    s32 position_count = 0;
    s32 position_capacity = 0;
    s32 normal_count = 0;
    s32 normal_capacity = 0;
    s32 uv_count = 0;
    s32 uv_capacity = 0;

    vertex->row_count = 0;

    if (file_data == NULL)
    {
        return false;
    }

    // SDL_LoadFile terminates the data with a null byte, so the file can be
    // cut into lines in place.
    line = file_data;

    while (*line != '\0')
    {
        char* line_end = strchr(line, '\n');
        char* next_line = line_end != NULL ? line_end + 1 : line + strlen(line);
        f32 values[3];

        if (line_end != NULL)
        {
            *line_end = '\0';
        }

        if (strncmp(line, "v ", 2) == 0)
        {
            if (sscanf(line, "v %f %f %f", &values[0], &values[1], &values[2]) == 3)
            {
                mesh_append_f32(&position_array, &position_count, &position_capacity, values, 3);
            }
        }
        else if (strncmp(line, "vn ", 3) == 0)
        {
            if (sscanf(line, "vn %f %f %f", &values[0], &values[1], &values[2]) == 3)
            {
                mesh_append_f32(&normal_array, &normal_count, &normal_capacity, values, 3);
            }
        }
        else if (strncmp(line, "vt ", 3) == 0)
        {
            if (sscanf(line, "vt %f %f", &values[0], &values[1]) == 2)
            {
                mesh_append_f32(&uv_array, &uv_count, &uv_capacity, values, 2);
            }
        }
        else if (strncmp(line, "f ", 2) == 0)
        {
            int p[3];
            int n[3];
            int uv[3];

            const int scanned = sscanf(
                line, "f %d/%d/%d %d/%d/%d %d/%d/%d",
                &p[0], &uv[0], &n[0], &p[1], &uv[1], &n[1], &p[2], &uv[2], &n[2]
            );

            int is_valid = scanned == 9;
            s32 index;

            for (index = 0; is_valid && index < 3; ++index)
            {
                is_valid =
                    p[index] >= 1 && p[index] <= position_count &&
                    n[index] >= 1 && n[index] <= normal_count &&
                    uv[index] >= 1 && uv[index] <= uv_count;
            }

            if (is_valid)
            {
                const s32 capacity = table_grow_capacity(vertex->row_capacity, vertex->row_count + 3);

                if (capacity != vertex->row_capacity)
                {
                    vertex->vertex = (ModelVertex*)column_resize(vertex->vertex, sizeof(ModelVertex), capacity);
                    vertex->row_capacity = capacity;
                }

                for (index = 0; index < 3; ++index)
                {
                    const f32* position = &position_array[(p[index] - 1) * 3];
                    const f32* normal = &normal_array[(n[index] - 1) * 3];
                    const f32* texture_uv = &uv_array[(uv[index] - 1) * 2];
                    ModelVertex* model_vertex = &vertex->vertex[vertex->row_count++];

                    model_vertex->position[0] = position[0];
                    model_vertex->position[1] = position[1];
                    model_vertex->position[2] = position[2];

                    model_vertex->normal[0] = normal[0];
                    model_vertex->normal[1] = normal[1];
                    model_vertex->normal[2] = normal[2];

                    model_vertex->uv[0] = texture_uv[0];
                    model_vertex->uv[1] = texture_uv[1];
                }
            }
        }

        line = next_line;
    }

    column_free(position_array);
    column_free(normal_array);
    column_free(uv_array);

    SDL_free(file_data);

    return true;
}

// Documented in src/app/mesh.h.
void
sector_quad_table_free(SectorQuadTable* table)
{
    column_free(table->local_x);
    column_free(table->local_y);
    column_free(table->local_z);
    column_free(table->direction);
    column_free(table->face_type);

    memset(table, 0, sizeof(*table));
}

// Documented in src/app/mesh.h.
void
voxel_vertex_table_free(VoxelVertexTable* table)
{
    column_free(table->vertex);

    memset(table, 0, sizeof(*table));
}

// Documented in src/app/mesh.h.
void
model_vertex_table_free(ModelVertexTable* table)
{
    column_free(table->vertex);

    memset(table, 0, sizeof(*table));
}
