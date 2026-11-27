// ============================================================================
// File:    tests/test_core.c
// Layer:   test
// Purpose: Checks of the core layer: table primitives, random numbers and
//          geometry.
// ============================================================================

#include "test.h"

#include <string.h>

#include "core/color.h"
#include "core/geometry.h"
#include "core/random.h"
#include "core/table.h"

// ----------------------------------------------------------------------------
// Function: test_core_table
// Purpose:  Checks capacity growth, key sequences, key lookup and the row
//           open and close primitives that keep clustered tables sorted.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_table(void)
{
    static const s32 key_array[5] = { 2, 3, 7, 11, 40 };
    Sequence sequence;
    s32 *column;

    TEST_CHECK_S32(table_grow_capacity(0, 0), TABLE_CAPACITY_MIN);
    TEST_CHECK_S32(table_grow_capacity(0, 1), TABLE_CAPACITY_MIN);
    TEST_CHECK_S32(table_grow_capacity(16, 16), 16);
    TEST_CHECK_S32(table_grow_capacity(16, 17), 32);
    TEST_CHECK_S32(table_grow_capacity(16, 100), 128);
    TEST_CHECK_S32(table_grow_capacity(64, 3), 64);

    sequence.next_value = 0;

    TEST_CHECK_S32(sequence_next(&sequence), 0);
    TEST_CHECK_S32(sequence_next(&sequence), 1);
    TEST_CHECK_S32(sequence_next(&sequence), 2);

    TEST_CHECK_S32(key_column_find_row(key_array, 5, 2), 0);
    TEST_CHECK_S32(key_column_find_row(key_array, 5, 11), 3);
    TEST_CHECK_S32(key_column_find_row(key_array, 5, 40), 4);
    TEST_CHECK_S32(key_column_find_row(key_array, 5, 1), TABLE_ROW_NONE);
    TEST_CHECK_S32(key_column_find_row(key_array, 5, 8), TABLE_ROW_NONE);
    TEST_CHECK_S32(key_column_find_row(key_array, 5, 41), TABLE_ROW_NONE);
    TEST_CHECK_S32(key_column_find_row(key_array, 0, 2), TABLE_ROW_NONE);

    TEST_CHECK_S32(key_column_find_insert_row(key_array, 5, 0), 0);
    TEST_CHECK_S32(key_column_find_insert_row(key_array, 5, 7), 2);
    TEST_CHECK_S32(key_column_find_insert_row(key_array, 5, 8), 3);
    TEST_CHECK_S32(key_column_find_insert_row(key_array, 5, 99), 5);

    column = (s32 *)column_resize(NULL, sizeof(s32), 8);

    TEST_CHECK(column != NULL);

    memcpy(column, key_array, sizeof(key_array));

    column_open_row(column, sizeof(s32), 5, 2);
    column[2] = 5;

    TEST_CHECK_S32(column[0], 2);
    TEST_CHECK_S32(column[1], 3);
    TEST_CHECK_S32(column[2], 5);
    TEST_CHECK_S32(column[3], 7);
    TEST_CHECK_S32(column[5], 40);

    column_close_row(column, sizeof(s32), 6, 0);

    TEST_CHECK_S32(column[0], 3);
    TEST_CHECK_S32(column[1], 5);
    TEST_CHECK_S32(column[4], 40);

    // Growing keeps the rows already stored.
    column = (s32 *)column_resize(column, sizeof(s32), 1024);

    TEST_CHECK_S32(column[0], 3);
    TEST_CHECK_S32(column[4], 40);

    column_free(column);
}

// ----------------------------------------------------------------------------
// Function: test_core_random
// Purpose:  Checks the generator against the published MT19937 reference
//           values and the distributions against their contracts.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_random(void)
{
    static Random random;
    static Random other;
    s32 histogram[12];
    u32 value = 0;
    s32 index;

    // With the default seed 5489 the first outputs and the 10000th output of
    // MT19937 are fixed by its specification.
    random_seed(&random, 5489u);

    TEST_CHECK(random_next_u32(&random) == 3499211612u);
    TEST_CHECK(random_next_u32(&random) == 581869302u);
    TEST_CHECK(random_next_u32(&random) == 3890346734u);

    for (index = 3; index < 10000; ++index)
    {
        value = random_next_u32(&random);
    }

    TEST_CHECK(value == 4123659995u);

    // Equal seeds give equal sequences; different seeds do not.
    random_seed(&random, 1388u);
    random_seed(&other, 1388u);

    for (index = 0; index < 1000; ++index)
    {
        TEST_CHECK(random_uniform_s32(&random, -50, 50) == random_uniform_s32(&other, -50, 50));
    }

    random_seed(&other, 1389u);

    TEST_CHECK(random_next_u32(&random) != random_next_u32(&other));

    // Integer draws stay in range and reach every value of a small range.
    memset(histogram, 0, sizeof(histogram));
    random_seed(&random, 7u);

    for (index = 0; index < 12000; ++index)
    {
        const s32 draw = random_uniform_s32(&random, 3, 14);

        TEST_CHECK(draw >= 3 && draw <= 14);

        if (draw >= 3 && draw <= 14)
        {
            histogram[draw - 3]++;
        }
    }

    for (index = 0; index < 12; ++index)
    {
        TEST_CHECK(histogram[index] > 800 && histogram[index] < 1200);
    }

    // A range of one value consumes no draw.
    random_seed(&random, 99u);
    random_seed(&other, 99u);

    TEST_CHECK_S32(random_uniform_s32(&random, 5, 5), 5);
    TEST_CHECK(random_next_u32(&random) == random_next_u32(&other));

    for (index = 0; index < 2000; ++index)
    {
        const f32 draw = random_uniform_f32(&random, -120.0f, 120.0f);

        TEST_CHECK(draw >= -120.0f && draw <= 120.0f);
    }
}

// ----------------------------------------------------------------------------
// Function: test_core_scalar
// Purpose:  Checks the scalar helpers and angle interpolation.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_scalar(void)
{
    const SinCos quarter = f32_sin_cos(F32_PI / 2.0f);

    TEST_CHECK_F32(f32_to_radians(180.0f), F32_PI, 1e-6f);
    TEST_CHECK_F32(f32_to_degrees(F32_PI), 180.0f, 1e-4f);
    TEST_CHECK_F32(f32_square_root(2.25f), 1.5f, 0.0f);
    TEST_CHECK_F32(quarter.sin, 1.0f, 1e-6f);
    TEST_CHECK_F32(quarter.cos, 0.0f, 1e-6f);
    TEST_CHECK_F32(f32_floor(-0.5f), -1.0f, 0.0f);
    TEST_CHECK_F32(f32_ceil(2.1f), 3.0f, 0.0f);
    TEST_CHECK_F32(f32_abs(-3.5f), 3.5f, 0.0f);
    TEST_CHECK_F32(f32_min(1.0f, 2.0f), 1.0f, 0.0f);
    TEST_CHECK_F32(f32_max(1.0f, 2.0f), 2.0f, 0.0f);
    TEST_CHECK_F32(f32_clamp(5.0f, 0.0f, 1.0f), 1.0f, 0.0f);
    TEST_CHECK_F32(f32_clamp(-5.0f, 0.0f, 1.0f), 0.0f, 0.0f);
    TEST_CHECK_F32(f32_clamp(0.25f, 0.0f, 1.0f), 0.25f, 0.0f);
    TEST_CHECK(f32_infinity() > 3.0e38f);
    TEST_CHECK(-f32_infinity() < -3.0e38f);
    TEST_CHECK_S32(s32_min(-3, 4), -3);
    TEST_CHECK_S32(s32_max(-3, 4), 4);

    TEST_CHECK_F32(angle_get_shortest_delta(350.0f, 10.0f), 20.0f, 0.0f);
    TEST_CHECK_F32(angle_get_shortest_delta(10.0f, 350.0f), -20.0f, 0.0f);
    TEST_CHECK_F32(angle_get_shortest_delta(0.0f, 180.0f), 180.0f, 0.0f);

    // Interpolation moves towards the target without overshooting it and
    // stays put once it has arrived.
    TEST_CHECK(angle_interpolate_to(0.0f, 90.0f, 5.0f, 0.1f) > 0.0f);
    TEST_CHECK(angle_interpolate_to(0.0f, 90.0f, 5.0f, 0.1f) < 90.0f);
    TEST_CHECK_F32(angle_interpolate_to(90.0f, 90.0f, 5.0f, 0.1f), 90.0f, 0.0f);
}

// ----------------------------------------------------------------------------
// Function: test_core_vector
// Purpose:  Checks vector arithmetic, quaternions and orientations.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_vector(void)
{
    const Vec3 a = vec3_make(1.0f, 2.0f, 3.0f);
    const Vec3 b = vec3_make(-4.0f, 5.0f, 0.5f);
    const Vec3 sum = vec3_add(a, b);
    const Vec3 cross = vec3_cross(vec3_make(1.0f, 0.0f, 0.0f), vec3_make(0.0f, 1.0f, 0.0f));
    const Vec3 unit = vec3_normalize(vec3_make(3.0f, 0.0f, 4.0f));
    const Vec3 zero = vec3_normalize(vec3_make(0.0f, 0.0f, 0.0f));
    const Quaternion yaw_90 = quaternion_from_euler_degrees(0.0f, 0.0f, 90.0f);
    const Quaternion pitch_45 = quaternion_from_euler_degrees(45.0f, 0.0f, 0.0f);
    const Quaternion half = quaternion_spherical_interpolation_to(quaternion_identity(), yaw_90, 0.5f);
    const Quaternion yaw_45 = quaternion_from_euler_degrees(0.0f, 0.0f, 45.0f);
    const Quaternion quarter = quaternion_spherical_interpolation_to(quaternion_identity(), yaw_90, 0.25f);
    const Quaternion yaw_22 = quaternion_from_euler_degrees(0.0f, 0.0f, 22.5f);
    const Quaternion beyond = quaternion_spherical_interpolation_to(quaternion_identity(), yaw_90, 7.0f);
    const Quaternion general = quaternion_from_euler_degrees(30.0f, 0.0f, 60.0f);
    const Quaternion rolled = quaternion_from_euler_degrees(0.0f, 90.0f, 0.0f);
    const Quaternion product = quaternion_multiply(
        quaternion_make(1.0f, 2.0f, 3.0f, 4.0f),
        quaternion_make(5.0f, 6.0f, 7.0f, 8.0f)
    );
    const Vec3 general_forward = rotation_get_forward(vec3_make(30.0f, 0.0f, 60.0f));
    Vec3 forward;
    Vec3 right;
    Vec3 up;

    TEST_CHECK_F32(sum.x, -3.0f, 0.0f);
    TEST_CHECK_F32(sum.y, 7.0f, 0.0f);
    TEST_CHECK_F32(sum.z, 3.5f, 0.0f);
    TEST_CHECK_F32(vec3_subtract(a, b).x, 5.0f, 0.0f);
    TEST_CHECK_F32(vec3_scale(2.0f, a).z, 6.0f, 0.0f);
    TEST_CHECK_F32(vec3_dot(a, b), 7.5f, 0.0f);
    TEST_CHECK_F32(cross.z, 1.0f, 0.0f);
    TEST_CHECK_F32(vec3_length(vec3_make(3.0f, 0.0f, 4.0f)), 5.0f, 0.0f);
    TEST_CHECK_F32(unit.x, 0.6f, 1e-6f);
    TEST_CHECK_F32(unit.z, 0.8f, 1e-6f);
    TEST_CHECK_F32(zero.x + zero.y + zero.z, 0.0f, 0.0f);
    TEST_CHECK_F32(vec3_min(a, b).x, -4.0f, 0.0f);
    TEST_CHECK_F32(vec3_max(a, b).y, 5.0f, 0.0f);
    TEST_CHECK_S32(ivec2_add(ivec2_make(1, 2), ivec2_make(3, 4)).y, 6);
    TEST_CHECK_S32(ivec2_subtract(ivec2_make(1, 2), ivec2_make(3, 4)).x, -2);
    TEST_CHECK_F32(vec3_from_ivec3(ivec3_make(1, -2, 3)).y, -2.0f, 0.0f);

    // The world is Z-up: unrotated, forward is +X, right is -Y, up is +Z.
    forward = quaternion_get_forward(quaternion_identity());
    right = quaternion_get_right(quaternion_identity());
    up = quaternion_get_up(quaternion_identity());

    TEST_CHECK_F32(forward.x, 1.0f, 0.0f);
    TEST_CHECK_F32(right.y, -1.0f, 0.0f);
    TEST_CHECK_F32(up.z, 1.0f, 0.0f);

    // A yaw of 90 degrees turns forward to +Y; a positive pitch raises it.
    forward = quaternion_get_forward(yaw_90);

    TEST_CHECK_F32(forward.x, 0.0f, 1e-6f);
    TEST_CHECK_F32(forward.y, 1.0f, 1e-6f);

    forward = quaternion_get_forward(pitch_45);

    TEST_CHECK(forward.z > 0.7f && forward.x > 0.7f);

    forward = rotation_get_forward(vec3_make(0.0f, 0.0f, 90.0f));
    right = rotation_get_right(vec3_make(0.0f, 0.0f, 90.0f));

    TEST_CHECK_F32(forward.y, 1.0f, 1e-6f);
    TEST_CHECK_F32(right.x, 1.0f, 1e-6f);

    // The Hamilton product, on operands that exercise every term.
    TEST_CHECK_F32(product.x, 24.0f, 0.0f);
    TEST_CHECK_F32(product.y, 48.0f, 0.0f);
    TEST_CHECK_F32(product.z, 48.0f, 0.0f);
    TEST_CHECK_F32(product.w, -6.0f, 0.0f);

    // Quaternions and Euler rotations agree on where a combined pitch and
    // yaw faces, and its right-hand direction stays horizontal.
    forward = quaternion_get_forward(general);
    right = quaternion_get_right(general);

    TEST_CHECK_F32(forward.x, general_forward.x, 1e-6f);
    TEST_CHECK_F32(forward.y, general_forward.y, 1e-6f);
    TEST_CHECK_F32(forward.z, general_forward.z, 1e-6f);
    TEST_CHECK_F32(forward.z, 0.5f, 1e-6f);
    TEST_CHECK_F32(right.z, 0.0f, 1e-6f);
    TEST_CHECK_F32(vec3_dot(forward, right), 0.0f, 1e-6f);
    TEST_CHECK_F32(vec3_length(quaternion_rotate_vector(general, vec3_make(1.0f, 2.0f, 3.0f))), f32_square_root(14.0f), 1e-5f);

    // A quarter roll about the forward axis tips up over to the side and
    // leaves forward alone.
    forward = quaternion_get_forward(rolled);
    up = quaternion_get_up(rolled);

    TEST_CHECK_F32(forward.x, 1.0f, 1e-6f);
    TEST_CHECK_F32(up.z, 0.0f, 1e-6f);
    TEST_CHECK_F32(f32_abs(up.y), 1.0f, 1e-6f);

    // Halfway between no rotation and a quarter turn is an eighth turn.
    TEST_CHECK_F32(half.z, yaw_45.z, 1e-6f);
    TEST_CHECK_F32(half.w, yaw_45.w, 1e-6f);

    // A quarter of the way is a sixteenth turn, and the amount is clamped.
    TEST_CHECK_F32(quarter.z, yaw_22.z, 1e-6f);
    TEST_CHECK_F32(quarter.w, yaw_22.w, 1e-6f);
    TEST_CHECK_F32(beyond.z, yaw_90.z, 1e-6f);
    TEST_CHECK_F32(beyond.w, yaw_90.w, 1e-6f);
    TEST_CHECK_F32(quaternion_dot(yaw_90, yaw_90), 1.0f, 1e-6f);
    TEST_CHECK_F32(quaternion_normalize(quaternion_make(0.0f, 0.0f, 0.0f, 0.0f)).w, 1.0f, 0.0f);
    TEST_CHECK_F32(quaternion_negate(yaw_90).z, -yaw_90.z, 0.0f);
}

// ----------------------------------------------------------------------------
// Function: test_core_matrix
// Purpose:  Checks matrix construction, multiplication and the camera
//           matrices.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_matrix(void)
{
    const Mat4 identity = mat4_diagonal(1.0f);
    const Mat4 translation = mat4_translate(identity, vec3_make(1.0f, 2.0f, 3.0f));
    const Mat4 rotation = mat4_rotate(identity, F32_PI / 2.0f, vec3_make(0.0f, 0.0f, 2.0f));
    const Mat4 product = mat4_multiply(translation, rotation);
    const Mat4 same = mat4_multiply(identity, translation);
    const Mat4 view = mat4_view(vec3_make(5.0f, 6.0f, 7.0f), quaternion_identity());
    const Mat4 orthographic = mat4_orthographic(vec2_make(0.0f, 0.0f), vec2_make(800.0f, 600.0f), 0.0f, 1.0f);
    const Mat4 projection = mat4_projection(F32_PI / 3.0f, 1.5f, 0.1f, 1000.0f);
    s32 column;
    s32 row;

    TEST_CHECK(sizeof(Mat4) == 16 * sizeof(f32));

    for (column = 0; column < 4; ++column)
    {
        for (row = 0; row < 4; ++row)
        {
            TEST_CHECK_F32(identity.entry[column][row], column == row ? 1.0f : 0.0f, 0.0f);
            TEST_CHECK_F32(same.entry[column][row], translation.entry[column][row], 0.0f);
        }
    }

    // The translation lives in the fourth column.
    TEST_CHECK_F32(translation.entry[3][0], 1.0f, 0.0f);
    TEST_CHECK_F32(translation.entry[3][1], 2.0f, 0.0f);
    TEST_CHECK_F32(translation.entry[3][2], 3.0f, 0.0f);

    // A quarter turn about Z maps +X to +Y.
    TEST_CHECK_F32(rotation.entry[0][0], 0.0f, 1e-6f);
    TEST_CHECK_F32(rotation.entry[0][1], 1.0f, 1e-6f);
    TEST_CHECK_F32(rotation.entry[1][0], -1.0f, 1e-6f);
    TEST_CHECK_F32(product.entry[3][0], 1.0f, 0.0f);
    TEST_CHECK_F32(product.entry[0][1], 1.0f, 1e-6f);

    // The view matrix moves the camera position to the origin.
    TEST_CHECK_F32(
        view.entry[0][0] * 5.0f + view.entry[1][0] * 6.0f + view.entry[2][0] * 7.0f + view.entry[3][0],
        0.0f, 1e-5f
    );
    TEST_CHECK_F32(
        view.entry[0][2] * 5.0f + view.entry[1][2] * 6.0f + view.entry[2][2] * 7.0f + view.entry[3][2],
        0.0f, 1e-5f
    );

    // The orthographic matrix maps the top-left pixel to (-1, 1).
    TEST_CHECK_F32(orthographic.entry[3][0], -1.0f, 0.0f);
    TEST_CHECK_F32(orthographic.entry[3][1], 1.0f, 0.0f);
    TEST_CHECK_F32(orthographic.entry[0][0] * 800.0f + orthographic.entry[3][0], 1.0f, 1e-6f);
    TEST_CHECK_F32(orthographic.entry[1][1] * 600.0f + orthographic.entry[3][1], -1.0f, 1e-6f);

    TEST_CHECK_F32(projection.entry[2][3], -1.0f, 0.0f);
    TEST_CHECK_F32(projection.entry[1][1] / projection.entry[0][0], 1.5f, 1e-5f);
}

// ----------------------------------------------------------------------------
// Function: test_core_bounds
// Purpose:  Checks rectangle overlap, intersection and subtraction. The
//           subtraction is what keeps a floor partitioned when an area is
//           placed on top of others, so its pieces must tile exactly.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_core_bounds(void)
{
    const IBounds2 outer = ibounds2_make(0, 0, 10, 10);
    const IBounds2 inner = ibounds2_make(3, 4, 6, 8);
    const IBounds2 corner = ibounds2_make(8, 8, 20, 20);
    const IBounds2 touching = ibounds2_make(10, 0, 20, 10);
    const IBounds2 cover = ibounds2_make(-1, -1, 11, 11);
    IBounds2List list;
    s32 area;
    s32 index;
    s32 other;

    TEST_CHECK(ibounds2_overlaps(outer, inner));
    TEST_CHECK(ibounds2_overlaps(outer, corner));
    TEST_CHECK(!ibounds2_overlaps(outer, touching));
    TEST_CHECK_S32(ibounds2_size(inner).x, 3);
    TEST_CHECK_S32(ibounds2_size(inner).y, 4);
    TEST_CHECK_S32(ibounds2_intersection(outer, corner).min.x, 8);
    TEST_CHECK_S32(ibounds2_intersection(outer, corner).max.y, 10);

    // A hole in the middle leaves four pieces that tile the remainder.
    list = ibounds2_subtract(outer, inner);
    area = 0;

    TEST_CHECK_S32(list.count, 4);

    for (index = 0; index < list.count; ++index)
    {
        const IVec2 size = ibounds2_size(list.values[index]);

        TEST_CHECK(size.x > 0 && size.y > 0);
        TEST_CHECK(!ibounds2_overlaps(list.values[index], inner));

        for (other = index + 1; other < list.count; ++other)
        {
            TEST_CHECK(!ibounds2_overlaps(list.values[index], list.values[other]));
        }

        area += size.x * size.y;
    }

    TEST_CHECK_S32(area, 10 * 10 - 3 * 4);

    // A corner cut leaves two pieces; no overlap leaves the rectangle
    // itself; a full cover leaves nothing.
    list = ibounds2_subtract(outer, corner);

    TEST_CHECK_S32(list.count, 2);

    list = ibounds2_subtract(outer, touching);

    TEST_CHECK_S32(list.count, 1);
    TEST_CHECK_S32(list.values[0].max.x, 10);

    list = ibounds2_subtract(outer, cover);

    TEST_CHECK_S32(list.count, 0);
}

// Documented in tests/test.h.
void
test_core_run(void)
{
    TEST_CHECK_F32(COLOR_CYAN.g + COLOR_CYAN.b + COLOR_CYAN.a - COLOR_CYAN.r, 3.0f, 0.0f);
    TEST_CHECK_F32(color_make(0.1f, 0.2f, 0.3f, 0.4f).b, 0.3f, 0.0f);

    test_core_table();
    test_core_random();
    test_core_scalar();
    test_core_vector();
    test_core_matrix();
    test_core_bounds();
}
