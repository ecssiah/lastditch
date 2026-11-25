// ============================================================================
// File:    tests/test_presentation.c
// Layer:   test
// Purpose: Checks of the CPU side of presentation: the heads-up display
//          labels, the debug overlay, sector meshing and model loading.
// ============================================================================

#include "test.h"

#include <stdio.h>
#include <string.h>

#include "app/control.h"
#include "app/debug.h"
#include "app/mesh.h"
#include "app/screen.h"

// ----------------------------------------------------------------------------
// Constants: recorded presentation values
// Purpose:   Properties of the geometry generated from the world of
//            WORLD_SEED and from the shipped assets. Like the recorded
//            world values they are platform independent.
// ----------------------------------------------------------------------------
#define TEST_MESH_VERTEX_COUNT 9250824      // Vertices of all sector meshes together.
#define TEST_DEBUG_LINE_COUNT 77928         // Lines of the debug overlay.
#define TEST_MODEL_VERTEX_COUNT 720         // Vertices of assets/model/actor.obj.
#define TEST_PATH_SIZE 1100                 // Size of the buffers that hold asset paths.

// ----------------------------------------------------------------------------
// Function: test_presentation_screen
// Purpose:  Checks the heads-up display labels for camera positions on the
//           roof, inside the tower and outside the world.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_presentation_screen(void)
{
    static Screen screen;
    static Control control;

    memset(&screen, 0, sizeof(screen));
    memset(&control, 0, sizeof(control));

    control.position = vec3_make(128.0f, 116.0f, 98.6f);
    screen_update(&screen, &control);

    TEST_CHECK_S32(screen.label.row_count, 4);
    TEST_CHECK(strcmp(screen.label.text[0], "Position 128.0 116.0 98.6") == 0);
    TEST_CHECK(strcmp(screen.label.text[1], "Cell 128 116 98") == 0);
    TEST_CHECK(strcmp(screen.label.text[2], "Sector 4 3") == 0);
    TEST_CHECK(strcmp(screen.label.text[3], "Floor R-6") == 0);
    TEST_CHECK_F32(screen.label.x[0], 20.0f, 0.0f);
    TEST_CHECK_F32(screen.label.y[0], 20.0f, 0.0f);
    TEST_CHECK_F32(screen.label.y[1], 42.0f, 0.0f);
    TEST_CHECK_F32(screen.label.y[2], 64.0f, 0.0f);
    TEST_CHECK_F32(screen.label.y[3], 86.0f, 0.0f);

    control.position = vec3_make(40.25f, 200.75f, 50.0f);
    screen_update(&screen, &control);

    TEST_CHECK_S32(screen.label.row_count, 4);
    TEST_CHECK(strcmp(screen.label.text[0], "Position 40.2 200.8 50.0") == 0 || strcmp(screen.label.text[0], "Position 40.3 200.8 50.0") == 0);
    TEST_CHECK(strcmp(screen.label.text[1], "Cell 40 200 50") == 0);
    TEST_CHECK(strcmp(screen.label.text[2], "Sector 1 6") == 0);
    TEST_CHECK(strcmp(screen.label.text[3], "Floor T-3") == 0);

    // Outside the world the labels fall back to placeholders.
    control.position = vec3_make(-5.0f, 300.0f, -1.0f);
    screen_update(&screen, &control);

    TEST_CHECK(strcmp(screen.label.text[1], "Cell - - -") == 0);
    TEST_CHECK(strcmp(screen.label.text[2], "Sector - -") == 0);
    TEST_CHECK(strcmp(screen.label.text[3], "Floor -") == 0);

    control.position = vec3_make(10.0f, 10.0f, 300.0f);
    screen_update(&screen, &control);

    TEST_CHECK(strcmp(screen.label.text[1], "Cell - - -") == 0);
    TEST_CHECK(strcmp(screen.label.text[2], "Sector 0 0") == 0);
    TEST_CHECK(strcmp(screen.label.text[3], "Floor -") == 0);
}

// ----------------------------------------------------------------------------
// Function: test_presentation_debug
// Purpose:  Checks the debug overlay built from the generated world: its
//           size, its two colors and that every line lies inside the world.
// Params:   world - the generated world
// Returns:  nothing
// Reads:    world.area, world.link and their indexes
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_presentation_debug(const World* world)
{
    static Debug debug;
    s32 red_count = 0;
    s32 cyan_count = 0;
    s32 outside_count = 0;
    s32 box_count = 0;
    s32 floor_number;
    s32 row;

    memset(&debug, 0, sizeof(debug));

    debug_init(&debug, world);

    TEST_CHECK_S32(debug.line.row_count, 2 * TEST_DEBUG_LINE_COUNT);

    for (row = 0; row < debug.line.row_count; ++row)
    {
        const DebugVertex* vertex = &debug.line.vertex[row];
        const b32 is_red = vertex->color[0] == 1.0f && vertex->color[1] == 0.0f && vertex->color[2] == 0.0f;
        const b32 is_cyan = vertex->color[0] == 0.0f && vertex->color[1] == 1.0f && vertex->color[2] == 1.0f;

        red_count += is_red;
        cyan_count += is_cyan;

        outside_count +=
            vertex->position[0] < 0.0f || vertex->position[0] > (f32)WORLD_SIZE_IN_CELLS ||
            vertex->position[1] < 0.0f || vertex->position[1] > (f32)WORLD_SIZE_IN_CELLS ||
            vertex->position[2] < 0.0f || vertex->position[2] > (f32)SECTOR_HEIGHT_IN_CELLS;
    }

    TEST_CHECK_S32(outside_count, 0);
    TEST_CHECK_S32(red_count + cyan_count, debug.line.row_count);

    // One box (twelve lines) per area and one per end of each link; every
    // area and link lies on a floor that the overlay shows.
    for (floor_number = 0; floor_number < FLOOR_COUNT; ++floor_number)
    {
        s32 row_count;

        area_floor_index_get_rows(&world->area_floor_index, &world->area, floor_number, &row_count);

        box_count += row_count;
    }

    TEST_CHECK_S32(red_count, 24 * box_count);
    TEST_CHECK_S32(cyan_count, 24 * 2 * world->link.row_count);

    debug_update(&debug);
    debug_quit(&debug);

    TEST_CHECK(debug.line.vertex == NULL);
    TEST_CHECK_S32(debug.line.row_count, 0);
}

// ----------------------------------------------------------------------------
// Function: test_presentation_mesh_block
// Purpose:  Checks sector meshing on a hand-built world with known faces:
//           a lone compass block, which shows a different face type in
//           every direction, and two adjacent blocks, which hide the faces
//           between them.
// Params:   none
// Returns:  nothing
// Reads:    BLOCK_FACE_TYPE
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_presentation_mesh_block(void)
{
    static World world;
    SectorQuadTable quad;
    VoxelVertexTable vertex;
    const s32 sector_index = world_cell_coordinate_to_sector_index(70, 40);
    s32 row;

    memset(&world, 0, sizeof(world));
    memset(&quad, 0, sizeof(quad));
    memset(&vertex, 0, sizeof(vertex));
    cell_table_init(&world.cell);

    // Sector (2, 1) starts at cell (64, 32).
    world_set_block(&world, 70, 40, 200, BLOCK_TYPE_COMPASS);
    world_set_block(&world, 64, 32, 0, BLOCK_TYPE_METAL1);
    world_set_block(&world, 65, 32, 0, BLOCK_TYPE_METAL1);
    world_calculate_direction_masks(&world);

    mesh_build_sector_quads(&world, sector_index, &quad);

    // Two blocks share one pair of faces: 2 * 6 - 2 visible, plus the six
    // faces of the compass. Rows are ordered by cell, then by direction.
    TEST_CHECK_S32(quad.row_count, 10 + 6);

    for (row = 0; row < 6 && quad.row_count == 16; ++row)
    {
        const s32 compass_row = 10 + row;

        TEST_CHECK_S32(quad.local_x[compass_row], 6);
        TEST_CHECK_S32(quad.local_y[compass_row], 8);
        TEST_CHECK_S32(quad.local_z[compass_row], 200);
        TEST_CHECK_S32(quad.direction[compass_row], row);
        TEST_CHECK_S32(quad.face_type[compass_row], BLOCK_FACE_TYPE[BLOCK_TYPE_COMPASS][row]);
    }

    if (quad.row_count == 16)
    {
        // The first block shows every face but the east one.
        TEST_CHECK_S32(quad.direction[0], DIRECTION_WEST);
        TEST_CHECK_S32(quad.direction[4], DIRECTION_DOWN);
        TEST_CHECK_S32(quad.local_x[5], 1);
        TEST_CHECK_S32(quad.direction[5], DIRECTION_EAST);
    }

    mesh_build_sector_vertices(&quad, &vertex);

    TEST_CHECK_S32(vertex.row_count, 16 * SECTOR_VERTEX_COUNT_PER_QUAD);

    if (vertex.row_count == 16 * SECTOR_VERTEX_COUNT_PER_QUAD)
    {
        // The compass's east face: first corner at local (7, 8, 200).
        const VoxelVertex* east = &vertex.vertex[10 * SECTOR_VERTEX_COUNT_PER_QUAD];
        const u32 layer_plus_one = mesh_encode_face_texture_index(FACE_TYPE_DIRECTION_EAST);

        TEST_CHECK(east[0].vertex == (7u | (8u << 6) | (200u << 12)));
        TEST_CHECK(east[1].vertex == (7u | (9u << 6) | (200u << 12)));
        TEST_CHECK(east[2].vertex == (7u | (9u << 6) | (201u << 12)));
        TEST_CHECK(east[3].vertex == east[0].vertex);
        TEST_CHECK(east[4].vertex == east[2].vertex);
        TEST_CHECK(east[5].vertex == (7u | (8u << 6) | (201u << 12)));
        TEST_CHECK(east[0].face_type == (layer_plus_one | ((u32)DIRECTION_EAST << 8)));
    }

    // Face types before FACE_TYPE_NONE keep their index as texture layer;
    // the ones after it move down by one. The stored value is one-based.
    TEST_CHECK(mesh_encode_face_texture_index(FACE_TYPE_CARVED1) == 1u);
    TEST_CHECK(mesh_encode_face_texture_index((FaceType)(FACE_TYPE_NONE - 1)) == (u32)FACE_TYPE_NONE);
    TEST_CHECK(mesh_encode_face_texture_index((FaceType)(FACE_TYPE_NONE + 1)) == (u32)FACE_TYPE_NONE + 1u);
    TEST_CHECK(mesh_encode_face_texture_index((FaceType)(FACE_TYPE_COUNT - 1)) == (u32)FACE_TYPE_COUNT - 1u);

    // Every other sector is empty.
    mesh_build_sector_quads(&world, 0, &quad);
    mesh_build_sector_vertices(&quad, &vertex);

    TEST_CHECK_S32(quad.row_count, 0);
    TEST_CHECK_S32(vertex.row_count, 0);

    sector_quad_table_free(&quad);
    voxel_vertex_table_free(&vertex);
    world_quit(&world);

    TEST_CHECK(quad.local_x == NULL);
    TEST_CHECK(vertex.vertex == NULL);
}

// ----------------------------------------------------------------------------
// Function: test_presentation_mesh_world
// Purpose:  Checks the sector meshes of the generated world: every vertex
//           decodes to a position inside its sector, a valid direction and
//           an existing texture layer, and the total matches the record.
// Params:   world - the generated world
// Returns:  nothing
// Reads:    world.cell
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_presentation_mesh_world(const World* world)
{
    SectorQuadTable quad;
    VoxelVertexTable vertex;
    s32 total_vertex_count = 0;
    s32 invalid_count = 0;
    s32 sector_index;
    s32 row;

    memset(&quad, 0, sizeof(quad));
    memset(&vertex, 0, sizeof(vertex));

    for (sector_index = 0; sector_index < WORLD_AREA_IN_SECTORS; ++sector_index)
    {
        mesh_build_sector_quads(world, sector_index, &quad);
        mesh_build_sector_vertices(&quad, &vertex);

        TEST_CHECK_S32(vertex.row_count, quad.row_count * SECTOR_VERTEX_COUNT_PER_QUAD);

        for (row = 0; row < vertex.row_count; ++row)
        {
            const u32 packed = vertex.vertex[row].vertex;
            const u32 face = vertex.vertex[row].face_type;
            const u32 x = packed & 63u;
            const u32 y = (packed >> 6) & 63u;
            const u32 direction = (face >> 8) & 7u;
            const u32 layer_plus_one = face & 255u;

            invalid_count +=
                x > SECTOR_SIZE_IN_CELLS ||
                y > SECTOR_SIZE_IN_CELLS ||
                (packed >> 20) != 0 ||
                direction >= DIRECTION_COUNT ||
                layer_plus_one < 1 ||
                layer_plus_one > FACE_TYPE_COUNT - 1 ||
                (face >> 11) != 0;
        }

        total_vertex_count += vertex.row_count;
    }

    TEST_CHECK_S32(invalid_count, 0);
    TEST_CHECK_S32(total_vertex_count, TEST_MESH_VERTEX_COUNT);

    sector_quad_table_free(&quad);
    voxel_vertex_table_free(&vertex);
}

// ----------------------------------------------------------------------------
// Function: test_presentation_model
// Purpose:  Checks the model loader on a small file written by the test
//           and on the actor model shipped with the game.
// Params:   none
// Returns:  nothing
// Reads:    the asset directory; writes and removes a scratch file in the
//           working directory
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_presentation_model(void)
{
    static const char scratch_path[] = "lastditch_test_model.obj";

    static const char scratch_text[] =
        "# comment\n"
        "o Triangle\n"
        "v 0.0 0.0 0.0\n"
        "v 1.0 0.0 0.5\n"
        "v 0.0 2.0 -1.5\r\n"
        "vn 0.0 0.0 1.0\n"
        "vt 0.25 0.75\n"
        "vt 1.0 0.0\n"
        "s 0\n"
        "f 1/1/1 2/2/1 3/1/1\n"
        "f 3/2/1 2/1/1 1/1/1\n"
        "f 1/1/1 2/2/1 9/1/1\n"
        "f 1 2 3\n"
        "v 5.0 5.0";

    char path[TEST_PATH_SIZE];
    ModelVertexTable vertex;
    FILE* file;
    s32 row;

    memset(&vertex, 0, sizeof(vertex));

    TEST_CHECK(!mesh_load_model("no_such_directory/no_such_model.obj", &vertex));
    TEST_CHECK_S32(vertex.row_count, 0);

    file = fopen(scratch_path, "wb");

    TEST_CHECK(file != NULL);

    if (file != NULL)
    {
        fwrite(scratch_text, 1, sizeof(scratch_text) - 1, file);
        fclose(file);

        TEST_CHECK(mesh_load_model(scratch_path, &vertex));

        // Two complete faces; the face with an undefined position, the face
        // without texture coordinates and the incomplete vertex are skipped.
        TEST_CHECK_S32(vertex.row_count, 6);

        if (vertex.row_count == 6)
        {
            TEST_CHECK_F32(vertex.vertex[0].position[0], 0.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[1].position[0], 1.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[1].position[2], 0.5f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[2].position[1], 2.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[2].position[2], -1.5f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[0].normal[2], 1.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[0].uv[0], 0.25f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[0].uv[1], 0.75f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[1].uv[0], 1.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[3].position[1], 2.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[3].uv[0], 1.0f, 0.0f);
            TEST_CHECK_F32(vertex.vertex[5].position[0], 0.0f, 0.0f);
        }

        remove(scratch_path);
    }

    SDL_snprintf(path, sizeof(path), "%smodel/actor.obj", test_get_asset_directory());

    TEST_CHECK(mesh_load_model(path, &vertex));
    TEST_CHECK_S32(vertex.row_count, TEST_MODEL_VERTEX_COUNT);

    // The actor model fits inside the actors' colliders, give or take.
    for (row = 0; row < vertex.row_count; ++row)
    {
        const ModelVertex* model_vertex = &vertex.vertex[row];
        const f32 normal_length_squared =
            model_vertex->normal[0] * model_vertex->normal[0] +
            model_vertex->normal[1] * model_vertex->normal[1] +
            model_vertex->normal[2] * model_vertex->normal[2];

        TEST_CHECK(f32_abs(model_vertex->position[0]) <= 1.0f);
        TEST_CHECK(f32_abs(model_vertex->position[1]) <= 1.0f);
        TEST_CHECK(f32_abs(model_vertex->position[2]) <= 1.0f);
        TEST_CHECK_F32(normal_length_squared, 1.0f, 1e-2f);
        TEST_CHECK(model_vertex->uv[0] >= 0.0f && model_vertex->uv[0] <= 1.0f);
        TEST_CHECK(model_vertex->uv[1] >= 0.0f && model_vertex->uv[1] <= 1.0f);
    }

    model_vertex_table_free(&vertex);
}

// Documented in tests/test.h.
void
test_presentation_run(void)
{
    const World* world = test_get_world();

    test_presentation_screen();
    test_presentation_debug(world);
    test_presentation_mesh_block();
    test_presentation_mesh_world(world);
    test_presentation_model();
}
