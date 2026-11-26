// ============================================================================
// File:    tests/test_simulation.c
// Layer:   test
// Purpose: Checks of the simulation: the initial population, actor physics,
//          the work schedule and player control.
// ============================================================================

#include "test.h"

#include <string.h>

#include "app/action.h"
#include "app/control.h"
#include "app/physics.h"
#include "app/population.h"
#include "platform/platform.h"

// ----------------------------------------------------------------------------
// Constants: simulation test sizes
// Purpose:   Row counts and run lengths used by the simulation checks.
// ----------------------------------------------------------------------------
#define TEST_SPAWN_COUNT 33                 // Rows of TEST_SPAWN_TABLE: the judge and 8 agents per nation.
#define TEST_SIMULATION_STEP_COUNT 900      // Simulation steps run by the whole-population check: 30 seconds.

// ----------------------------------------------------------------------------
// Record:  TestSpawnRecord
// Purpose: Row type of TEST_SPAWN_TABLE.
// ----------------------------------------------------------------------------
typedef struct TestSpawnRecord
{
    s32 actor_id;                   // Key of the actor.
    NationType nation_type;         // Nation of the actor.
    s32 x;                          // Spawn position along X.
    s32 y;                          // Spawn position along Y.
    s32 z;                          // Spawn position along Z.
} TestSpawnRecord;

// ----------------------------------------------------------------------------
// Reference: TEST_SPAWN_TABLE
// Purpose:   The initial population, recorded from the log of an earlier
//            build of the application (logs/engine_2026_11_24_17_39_46.log).
//            population_init must reproduce it exactly, which pins down the
//            random number generator, the integer distribution and the
//            order in which the nations are spawned.
// Key:       actor_id
// Order:     rows in actor_id order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
static const TestSpawnRecord TEST_SPAWN_TABLE[TEST_SPAWN_COUNT] =
{
    {  0, NATION_TYPE_LION, 128, 116, 100 },
    {  1, NATION_TYPE_WOLF, 204, 130, 103 },
    {  2, NATION_TYPE_WOLF, 208, 129, 103 },
    {  3, NATION_TYPE_WOLF, 211, 124, 103 },
    {  4, NATION_TYPE_WOLF, 207, 132, 103 },
    {  5, NATION_TYPE_WOLF, 208, 123, 103 },
    {  6, NATION_TYPE_WOLF, 210, 122, 103 },
    {  7, NATION_TYPE_WOLF, 211, 127, 103 },
    {  8, NATION_TYPE_WOLF, 204, 122, 103 },
    {  9, NATION_TYPE_EAGLE,  52, 133, 103 },
    { 10, NATION_TYPE_EAGLE,  42, 122, 103 },
    { 11, NATION_TYPE_EAGLE,  45, 122, 103 },
    { 12, NATION_TYPE_EAGLE,  43, 124, 103 },
    { 13, NATION_TYPE_EAGLE,  50, 123, 103 },
    { 14, NATION_TYPE_EAGLE,  45, 132, 103 },
    { 15, NATION_TYPE_EAGLE,  46, 129, 103 },
    { 16, NATION_TYPE_EAGLE,  44, 131, 103 },
    { 17, NATION_TYPE_LION, 131,  53, 103 },
    { 18, NATION_TYPE_LION, 130,  44, 103 },
    { 19, NATION_TYPE_LION, 130,  53, 103 },
    { 20, NATION_TYPE_LION, 123,  53, 103 },
    { 21, NATION_TYPE_LION, 132,  45, 103 },
    { 22, NATION_TYPE_LION, 126,  44, 103 },
    { 23, NATION_TYPE_LION, 123,  47, 103 },
    { 24, NATION_TYPE_LION, 127,  53, 103 },
    { 25, NATION_TYPE_BEAR, 131, 208, 103 },
    { 26, NATION_TYPE_BEAR, 129, 203, 103 },
    { 27, NATION_TYPE_BEAR, 126, 208, 103 },
    { 28, NATION_TYPE_BEAR, 132, 207, 103 },
    { 29, NATION_TYPE_BEAR, 124, 212, 103 },
    { 30, NATION_TYPE_BEAR, 128, 212, 103 },
    { 31, NATION_TYPE_BEAR, 130, 209, 103 },
    { 32, NATION_TYPE_BEAR, 130, 202, 103 }
};

// ----------------------------------------------------------------------------
// Function: test_simulation_population
// Purpose:  Checks the initial population against the recorded spawn table
//           and the relation between the actor and decision tables.
// Params:   population - a freshly initialised population
// Returns:  nothing
// Reads:    population, TEST_SPAWN_TABLE
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_simulation_population(const Population* population)
{
    const ActorTable* actor = &population->actor;
    s32 row;

    TEST_CHECK_S32(actor->row_count, TEST_SPAWN_COUNT);
    TEST_CHECK_S32(actor->row_count, 1 + NATION_TYPE_COUNT * INITIAL_POPULATION_SIZE);
    TEST_CHECK_S32(population->decision.row_count, actor->row_count - 1);
    TEST_CHECK_S32(population->judge_id, 0);

    for (row = 0; row < actor->row_count && row < TEST_SPAWN_COUNT; ++row)
    {
        const TestSpawnRecord* spawn = &TEST_SPAWN_TABLE[row];
        const int is_judge = actor->actor_id[row] == population->judge_id;
        const Vec3 forward = quaternion_get_forward(actor->orientation[row]);
        const Vec2 heading = direction_from_angle(actor->movement_yaw[row]);

        TEST_CHECK_S32(actor->actor_id[row], spawn->actor_id);
        TEST_CHECK_S32(actor->nation_type[row], spawn->nation_type);
        TEST_CHECK_F32(actor->position[AXIS_X][row], (f32)spawn->x, 0.0f);
        TEST_CHECK_F32(actor->position[AXIS_Y][row], (f32)spawn->y, 0.0f);
        TEST_CHECK_F32(actor->position[AXIS_Z][row], (f32)spawn->z, 0.0f);
        TEST_CHECK_S32(actor->actor_type[row], is_judge ? ACTOR_TYPE_JUDGE : ACTOR_TYPE_AGENT);
        TEST_CHECK_F32(actor->move_speed[row], is_judge ? JUDGE_DEFAULT_MOVE_SPEED : ACTOR_DEFAULT_MOVE_SPEED, 0.0f);
        TEST_CHECK_F32(actor->position_target[AXIS_X][row], actor->position[AXIS_X][row], 0.0f);
        TEST_CHECK_F32(actor->rotation_target[AXIS_Z][row], actor->movement_yaw[row], 0.0f);
        TEST_CHECK(actor->movement_yaw[row] >= 0.0f && actor->movement_yaw[row] <= 360.0f);
        TEST_CHECK_S32(actor->collision_enabled[row], 1);
        TEST_CHECK_S32(actor->is_grounded[row], 0);

        // The orientation faces along the heading.
        TEST_CHECK_F32(forward.x, heading.x, 1e-5f);
        TEST_CHECK_F32(forward.y, heading.y, 1e-5f);
        TEST_CHECK_F32(forward.z, 0.0f, 1e-5f);
    }

    // Exactly the agents steer themselves.
    for (row = 0; row < population->decision.row_count; ++row)
    {
        const s32 actor_row = actor_table_find_row(actor, population->decision.actor_id[row]);

        TEST_CHECK(actor_row != TABLE_ROW_NONE);
        TEST_CHECK(population->decision.actor_id[row] != population->judge_id);
        TEST_CHECK_S32(population->decision.decision_timer[row], 0);

        if (actor_row != TABLE_ROW_NONE)
        {
            TEST_CHECK_S32(actor->actor_type[actor_row], ACTOR_TYPE_AGENT);
        }
    }
}

// ----------------------------------------------------------------------------
// Function: test_simulation_add_actor
// Purpose:  Inserts a motionless actor with the judge's collider into a
//           table, for the physics checks.
// Params:   actor    - table to insert into
//           position - position of the new actor
// Returns:  The row of the new actor.
// Reads:    arguments only
// Writes:   actor
// ----------------------------------------------------------------------------
static s32
test_simulation_add_actor(ActorTable* actor, const Vec3 position)
{
    ActorRecord record;

    memset(&record, 0, sizeof(record));

    record.actor_id = sequence_next(&actor->id_sequence);
    record.actor_type = ACTOR_TYPE_AGENT;
    record.nation_type = NATION_TYPE_WOLF;
    record.move_speed = 1.0f;
    record.turn_speed = 5.0f;
    record.position = position;
    record.position_target = position;
    record.orientation = quaternion_identity();
    record.collision_enabled = true;
    record.collider_radius = vec3_make(0.30f, 0.30f, 0.90f);

    return actor_table_insert(actor, &record);
}

// ----------------------------------------------------------------------------
// Function: test_simulation_physics
// Purpose:  Checks gravity and collision on a small hand-built world: a
//           slab to land on, a wall to walk into and a ceiling to hit.
// Params:   none
// Returns:  nothing
// Reads:    nothing
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_simulation_physics(void)
{
    static World world;
    ActorTable actor;
    s32 faller;
    s32 walker;
    s32 jumper;
    s32 diver;
    s32 ghost;
    s32 glider;
    s32 step;

    memset(&world, 0, sizeof(world));
    memset(&actor, 0, sizeof(actor));
    cell_table_init(&world.cell);

    // A slab whose top surface is at z = 11, a wall at x = 60 and a
    // ceiling whose underside is at z = 15 over part of the slab.
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, ivec3_make(40, 40, 10), ivec3_make(40, 40, 1), BLOCK_TYPE_SMOOTH1);
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, ivec3_make(60, 40, 11), ivec3_make(1, 40, 6), BLOCK_TYPE_SMOOTH2);
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, ivec3_make(40, 40, 15), ivec3_make(10, 10, 1), BLOCK_TYPE_SMOOTH3);

    faller = test_simulation_add_actor(&actor, vec3_make(55.5f, 70.5f, 14.0f));
    walker = test_simulation_add_actor(&actor, vec3_make(55.5f, 60.5f, 11.9f));
    jumper = test_simulation_add_actor(&actor, vec3_make(45.5f, 45.5f, 11.9f));
    diver = test_simulation_add_actor(&actor, vec3_make(50.5f, 70.5f, 200.0f));
    ghost = test_simulation_add_actor(&actor, vec3_make(70.5f, 70.5f, 14.0f));

    actor.velocity[AXIS_X][walker] = 6.0f;
    actor.velocity[AXIS_Z][jumper] = ACTOR_DEFAULT_JUMP_SPEED;
    actor.velocity[AXIS_Z][diver] = -MAX_VELOCITY;
    actor.collision_enabled[ghost] = 0;

    // The first step applies gravity before moving.
    physics_update_actor(&world, &actor, faller);

    TEST_CHECK_F32(actor.velocity[AXIS_Z][faller], FIXED_FRAME_TIME_32 * FALLING_GRAVITY_MODIFIER * GRAVITY, 1e-5f);
    TEST_CHECK(actor.position[AXIS_Z][faller] < 14.0f);
    TEST_CHECK_S32(actor.is_grounded[faller], 0);

    for (step = 0; step < 120; ++step)
    {
        physics_update_actor(&world, &actor, faller);
        physics_update_actor(&world, &actor, walker);
        physics_update_actor(&world, &actor, jumper);
        physics_update_actor(&world, &actor, diver);
        physics_update_actor(&world, &actor, ghost);
    }

    // The faller rests on the slab: collider bottom exactly on its surface.
    TEST_CHECK_F32(actor.position[AXIS_Z][faller], 11.0f + 0.90f, 0.0f);
    TEST_CHECK_F32(actor.position[AXIS_X][faller], 55.5f, 0.0f);
    TEST_CHECK_F32(actor.velocity[AXIS_Z][faller], 0.0f, 0.0f);
    TEST_CHECK_S32(actor.is_grounded[faller], 1);

    // The walker stops with its collider against the wall and stays
    // grounded; its velocity along X is cleared.
    TEST_CHECK_F32(actor.position[AXIS_X][walker], 60.0f - 0.30f, 0.0f);
    TEST_CHECK_F32(actor.velocity[AXIS_X][walker], 0.0f, 0.0f);
    TEST_CHECK_F32(actor.position[AXIS_Z][walker], 11.9f, 1e-5f);
    TEST_CHECK_S32(actor.is_grounded[walker], 1);

    // The jumper hits the ceiling, falls back and lands where it started.
    TEST_CHECK_F32(actor.position[AXIS_Z][jumper], 11.9f, 1e-5f);
    TEST_CHECK_S32(actor.is_grounded[jumper], 1);

    // Even at terminal velocity an actor cannot pass through the slab.
    TEST_CHECK_F32(actor.position[AXIS_Z][diver], 11.9f, 1e-5f);
    TEST_CHECK_S32(actor.is_grounded[diver], 1);

    // Without collision an actor falls through everything.
    TEST_CHECK(actor.position[AXIS_Z][ghost] < 0.0f);
    TEST_CHECK_S32(actor.is_grounded[ghost], 0);

    // Hitting the ceiling stops upward motion at its underside.
    actor.velocity[AXIS_Z][jumper] = ACTOR_DEFAULT_JUMP_SPEED;
    physics_update_actor(&world, &actor, jumper);
    physics_update_actor(&world, &actor, jumper);
    physics_update_actor(&world, &actor, jumper);
    physics_update_actor(&world, &actor, jumper);

    TEST_CHECK(actor.position[AXIS_Z][jumper] <= 15.0f - 0.90f + 1e-5f);
    TEST_CHECK(actor.velocity[AXIS_Z][jumper] <= 0.0f);

    // Sub-stepping: an actor that moves three cells sideways in one step
    // while falling must not skip over a low wall whose top it starts
    // above. The wall occupies x = 30, z = 11 and 12; the actor starts with
    // its collider bottom at 13.2 and sinks below the wall's top before it
    // reaches it, so it must be stopped by the wall's side.
    world_set_shape(&world, STRUCTURE_SHAPE_CUBE, ivec3_make(30, 0, 11), ivec3_make(1, 40, 2), BLOCK_TYPE_SMOOTH4);

    glider = test_simulation_add_actor(&actor, vec3_make(28.0f, 20.5f, 13.2f + 0.90f));

    actor.velocity[AXIS_X][glider] = 90.0f;
    actor.velocity[AXIS_Z][glider] = -60.0f;

    physics_update_actor(&world, &actor, glider);

    TEST_CHECK_F32(actor.position[AXIS_X][glider], 30.0f - 0.30f, 0.0f);
    TEST_CHECK_F32(actor.velocity[AXIS_X][glider], 0.0f, 0.0f);
    TEST_CHECK(actor.position[AXIS_Z][glider] - 0.90f < 13.0f);

    actor_table_free(&actor);
    world_quit(&world);
}

// ----------------------------------------------------------------------------
// Function: test_simulation_decisions
// Purpose:  Checks the decision system and its place in the work schedule
//           on a two-actor population: only actors with a decision row are
//           steered, timers count down once per decision step, and an
//           expired timer restarts within its range.
// Params:   world - the generated world, needed to run the schedule
// Returns:  nothing
// Reads:    world, WORK_TABLE
// Writes:   the check counters
// ----------------------------------------------------------------------------
static void
test_simulation_decisions(World* world)
{
    static Population population;
    static Action action;
    ActorTable* actor = &population.actor;
    s32 bystander;
    s32 agent;
    s32 step;
    s32 timer_after_restart;

    memset(&population, 0, sizeof(population));
    random_seed(&population.random, POPULATION_SEED);

    // Both stand on the roof; only the second has a decision row.
    bystander = test_simulation_add_actor(actor, vec3_make(100.5f, 100.5f, (f32)ROOF_Z + 1.9f));
    agent = test_simulation_add_actor(actor, vec3_make(110.5f, 100.5f, (f32)ROOF_Z + 1.9f));

    population.judge_id = actor->actor_id[bystander];

    actor->velocity[AXIS_X][bystander] = 0.25f;
    actor->rotation_target[AXIS_Z][agent] = 90.0f;

    decision_table_insert(&population.decision, actor->actor_id[agent], 3);

    action_init(&action, world);

    // The decision system runs on every fourth step.
    for (step = 1; step <= 12; ++step)
    {
        action_update(&action, world, &population);

        TEST_CHECK_S32(population.decision.decision_timer[0], 3 - step / 4);
    }

    // Orientation runs on every step: the heading has been turning towards
    // the target all along, without reaching it yet.
    TEST_CHECK(actor->movement_yaw[agent] > 60.0f && actor->movement_yaw[agent] < 90.0f);
    TEST_CHECK_F32(actor->movement_yaw[bystander], 0.0f, 0.0f);

    // The next decision step finds the timer expired and restarts it.
    for (step = 13; step <= 16; ++step)
    {
        action_update(&action, world, &population);
    }

    timer_after_restart = population.decision.decision_timer[0];

    TEST_CHECK(timer_after_restart >= 10 && timer_after_restart <= 50);

    // The actor without a decision row was never steered: it kept the
    // velocity it was given and moved along it.
    TEST_CHECK_F32(actor->velocity[AXIS_X][bystander], 0.25f, 0.0f);
    TEST_CHECK_F32(actor->position[AXIS_X][bystander], 100.5f + 16.0f * FIXED_FRAME_TIME_32 * 0.25f, 1e-4f);
    TEST_CHECK_S32(actor->is_grounded[bystander], 1);

    // Once the agent has finished turning, every expired timer either sends
    // it walking at its move speed or stops it.
    for (step = 0; step < 2000; ++step)
    {
        const f32 speed_squared =
            actor->velocity[AXIS_X][agent] * actor->velocity[AXIS_X][agent] +
            actor->velocity[AXIS_Y][agent] * actor->velocity[AXIS_Y][agent];

        TEST_CHECK(speed_squared < 1e-6f || f32_abs(speed_squared - 1.0f) < 1e-4f);
        TEST_CHECK(actor->rotation_target[AXIS_Z][agent] >= 0.0f && actor->rotation_target[AXIS_Z][agent] <= 360.0f);

        action_update(&action, world, &population);
    }

    population_quit(&population);
}

// ----------------------------------------------------------------------------
// Function: test_simulation_schedule
// Purpose:  Checks the work schedule arithmetic and a long run of the whole
//           simulation on the generated world: every actor must end up
//           standing on a block, outside every block and inside the world.
// Params:   world      - the generated world; the simulation must leave it
//                        unchanged
//           population - a freshly initialised population; advanced by
//                        TEST_SIMULATION_STEP_COUNT steps
// Returns:  nothing
// Reads:    world, WORK_TABLE
// Writes:   population; the check counters
// ----------------------------------------------------------------------------
static void
test_simulation_schedule(World* world, Population* population)
{
    static Action action;
    const ActorTable* actor = &population->actor;
    const size_t cell_byte_count = (size_t)WORLD_VOLUME_IN_CELLS * sizeof(BlockType);
    const u64 block_hash_before = test_hash_bytes(test_hash_begin(), world->cell.block_type, cell_byte_count);
    const u32 area_revision_before = world->area.revision;
    WorkRecord work;
    s32 moved_count = 0;
    s32 judge_row;
    s32 step;
    s32 row;

    action_init(&action, world);

    TEST_CHECK(action.tick_count == 0);

    work.frequency = 4;
    work.phase = 0;
    work.work_function = NULL;

    action.tick_count = 8;
    TEST_CHECK(action_is_due(&action, &work));
    action.tick_count = 9;
    TEST_CHECK(!action_is_due(&action, &work));

    work.phase = 3;
    TEST_CHECK(action_is_due(&action, &work));

    work.frequency = 1;
    TEST_CHECK(action_is_due(&action, &work));

    action.tick_count = 0;

    for (step = 0; step < TEST_SIMULATION_STEP_COUNT; ++step)
    {
        action_update(&action, world, population);
    }

    TEST_CHECK(action.tick_count == TEST_SIMULATION_STEP_COUNT);

    // The scheduled systems read the world but never write to it.
    TEST_CHECK(test_hash_bytes(test_hash_begin(), world->cell.block_type, cell_byte_count) == block_hash_before);
    TEST_CHECK(world->area.revision == area_revision_before);

    // The judge has no decision row: it only fell straight down and stands
    // where it was spawned. Its fall uses only sums and products, so the
    // resting height is exact on every platform.
    judge_row = actor_table_find_row(actor, population->judge_id);

    TEST_CHECK_F32(actor->position[AXIS_X][judge_row], WORLD_CENTER_F32, 0.0f);
    TEST_CHECK_F32(actor->position[AXIS_Y][judge_row], WORLD_CENTER_F32 - 12.0f, 0.0f);
    TEST_CHECK_F32(actor->position[AXIS_Z][judge_row], (f32)ROOF_Z + 1.0f + 0.90f, 0.0f);
    TEST_CHECK_F32(actor->movement_yaw[judge_row], 90.0f, 0.0f);

    for (row = 0; row < actor->row_count; ++row)
    {
        const Vec3 position = actor_table_get_position(actor, row);
        const TestSpawnRecord* spawn = &TEST_SPAWN_TABLE[row];
        const IVec3 feet = world_position_to_cell_coordinate(position.x, position.y, position.z - actor->collider_radius[AXIS_Z][row] - 0.5f);
        const IVec3 body = world_position_to_cell_coordinate(position.x, position.y, position.z);

        TEST_CHECK(world_cell_coordinate_is_valid(body.x, body.y, body.z));
        TEST_CHECK_S32(actor->is_grounded[row], 1);
        TEST_CHECK(!world_is_solid(world, body.x, body.y, body.z));
        TEST_CHECK(actor->movement_yaw[row] >= 0.0f && actor->movement_yaw[row] <= 360.0f);

        // A grounded actor has a block under its collider. Its center may
        // overhang an edge, so the block is searched under the whole
        // footprint of the collider.
        {
            const s32 min_x = (s32)f32_floor(position.x - actor->collider_radius[AXIS_X][row]);
            const s32 max_x = (s32)f32_floor(position.x + actor->collider_radius[AXIS_X][row]);
            const s32 min_y = (s32)f32_floor(position.y - actor->collider_radius[AXIS_Y][row]);
            const s32 max_y = (s32)f32_floor(position.y + actor->collider_radius[AXIS_Y][row]);
            int is_supported = false;
            s32 x;
            s32 y;

            for (y = min_y; y <= max_y; ++y)
            {
                for (x = min_x; x <= max_x; ++x)
                {
                    is_supported = is_supported || world_is_solid(world, x, y, feet.z);
                }
            }

            TEST_CHECK(is_supported);
        }

        moved_count +=
            f32_abs(position.x - (f32)spawn->x) > 0.5f ||
            f32_abs(position.y - (f32)spawn->y) > 0.5f;
    }

    // Agents wander: in thirty seconds most of them have left their spawn
    // cell. (The exact paths depend on the platform's math library, so
    // only the tendency is checked.)
    TEST_CHECK(moved_count > (TEST_SPAWN_COUNT - 1) / 2);
}

// ----------------------------------------------------------------------------
// Function: test_simulation_press
// Purpose:  Advances a fake platform by one frame with a given set of held
//           buttons, the way platform_begin_frame would.
// Params:   platform - platform state to advance
//           button_a - first button held this frame, or BUTTON_TYPE_NONE
//           button_b - second button held this frame, or BUTTON_TYPE_NONE
// Returns:  nothing
// Reads:    arguments only
// Writes:   platform.button_current, platform.button_previous,
//           platform.pointer_delta_x, platform.pointer_delta_y
// ----------------------------------------------------------------------------
static void
test_simulation_press(Platform* platform, const ButtonType button_a, const ButtonType button_b)
{
    memcpy(platform->button_previous, platform->button_current, sizeof(platform->button_previous));
    memset(platform->button_current, 0, sizeof(platform->button_current));

    platform->button_current[button_a] = 1;
    platform->button_current[button_b] = 1;
    platform->button_current[BUTTON_TYPE_NONE] = 0;

    platform->pointer_delta_x = 0.0;
    platform->pointer_delta_y = 0.0;
}

// ----------------------------------------------------------------------------
// Function: test_simulation_control
// Purpose:  Checks player control with a fake platform: button edges,
//           driving and turning the judge, jumping, the possession toggle
//           and the free camera.
// Params:   population - a freshly initialised population; modified
// Returns:  nothing
// Reads:    nothing
// Writes:   population; the check counters
// ----------------------------------------------------------------------------
static void
test_simulation_control(Population* population)
{
    static Platform platform;
    static Control control;
    ActorTable* actor = &population->actor;
    const s32 judge_row = actor_table_find_row(actor, population->judge_id);
    Vec3 before;

    memset(&platform, 0, sizeof(platform));
    platform.aspect_ratio = 2.0f;

    // Button edges.
    test_simulation_press(&platform, BUTTON_TYPE_W, BUTTON_TYPE_NONE);

    TEST_CHECK(platform_button_is_down(&platform, BUTTON_TYPE_W));
    TEST_CHECK(platform_button_is_pressed(&platform, BUTTON_TYPE_W));
    TEST_CHECK(!platform_button_is_released(&platform, BUTTON_TYPE_W));

    test_simulation_press(&platform, BUTTON_TYPE_W, BUTTON_TYPE_NONE);

    TEST_CHECK(platform_button_is_down(&platform, BUTTON_TYPE_W));
    TEST_CHECK(!platform_button_is_pressed(&platform, BUTTON_TYPE_W));

    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);

    TEST_CHECK(!platform_button_is_down(&platform, BUTTON_TYPE_W));
    TEST_CHECK(platform_button_is_released(&platform, BUTTON_TYPE_W));

    control_init(&control, population);

    TEST_CHECK_S32(control.actor_id, population->judge_id);
    TEST_CHECK_F32(control.view_matrix.entry[0][0], 1.0f, 0.0f);

    // With nothing pressed the camera sits at the judge's eyes and the
    // projection follows the platform's aspect ratio.
    control_update(&control, &platform, population);

    TEST_CHECK_F32(control.position.x, actor->position[AXIS_X][judge_row], 0.0f);
    TEST_CHECK_F32(control.position.z, actor->position[AXIS_Z][judge_row] + 0.7f, 0.0f);
    TEST_CHECK_F32(control.projection_matrix.entry[1][1] / control.projection_matrix.entry[0][0], 2.0f, 1e-5f);
    TEST_CHECK_F32(actor->velocity[AXIS_X][judge_row], 0.0f, 0.0f);
    TEST_CHECK_F32(actor->velocity[AXIS_Y][judge_row], 0.0f, 0.0f);

    // The judge faces +Y (yaw 90): forward is +Y, strafing right is +X,
    // and a diagonal is no faster than a straight line.
    test_simulation_press(&platform, BUTTON_TYPE_W, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_X][judge_row], 0.0f, 1e-5f);
    TEST_CHECK_F32(actor->velocity[AXIS_Y][judge_row], JUDGE_DEFAULT_MOVE_SPEED, 1e-5f);

    test_simulation_press(&platform, BUTTON_TYPE_D, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_X][judge_row], JUDGE_DEFAULT_MOVE_SPEED, 1e-5f);
    TEST_CHECK_F32(actor->velocity[AXIS_Y][judge_row], 0.0f, 1e-5f);

    test_simulation_press(&platform, BUTTON_TYPE_S, BUTTON_TYPE_A);
    control_update(&control, &platform, population);

    TEST_CHECK(actor->velocity[AXIS_X][judge_row] < 0.0f && actor->velocity[AXIS_Y][judge_row] < 0.0f);
    TEST_CHECK_F32(
        actor->velocity[AXIS_X][judge_row] * actor->velocity[AXIS_X][judge_row] +
        actor->velocity[AXIS_Y][judge_row] * actor->velocity[AXIS_Y][judge_row],
        JUDGE_DEFAULT_MOVE_SPEED * JUDGE_DEFAULT_MOVE_SPEED, 1e-3f
    );

    // Pointer motion turns the rotation target while moving forward.
    test_simulation_press(&platform, BUTTON_TYPE_W, BUTTON_TYPE_NONE);
    platform.pointer_delta_x = 100.0;
    platform.pointer_delta_y = -50.0;
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_Y][judge_row], JUDGE_DEFAULT_MOVE_SPEED, 1e-5f);
    TEST_CHECK_F32(actor->rotation_target[AXIS_Z][judge_row], 90.0f - 100.0f * CAMERA_SENSITIVITY_X, 1e-4f);
    TEST_CHECK_F32(actor->rotation_target[AXIS_X][judge_row], 50.0f * CAMERA_SENSITIVITY_Y, 1e-4f);
    TEST_CHECK_F32(control.rotation.z, actor->rotation_target[AXIS_Z][judge_row], 0.0f);

    // Pitch is limited.
    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    platform.pointer_delta_y = -10000.0;
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->rotation_target[AXIS_X][judge_row], CAMERA_PITCH_LIMIT, 0.0f);

    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    platform.pointer_delta_y = 30000.0;
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->rotation_target[AXIS_X][judge_row], -CAMERA_PITCH_LIMIT, 0.0f);

    // Pointer motion below the dead zone is ignored.
    before = actor_table_get_rotation_target(actor, judge_row);
    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    platform.pointer_delta_x = 0.000001;
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->rotation_target[AXIS_Z][judge_row], before.z, 0.0f);

    // A jump needs the actor to be grounded and fires on the press only.
    actor->is_grounded[judge_row] = 0;
    test_simulation_press(&platform, BUTTON_TYPE_SPACE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_Z][judge_row], 0.0f, 0.0f);

    actor->is_grounded[judge_row] = 1;
    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);
    test_simulation_press(&platform, BUTTON_TYPE_SPACE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_Z][judge_row], ACTOR_DEFAULT_JUMP_SPEED, 0.0f);

    actor->velocity[AXIS_Z][judge_row] = 1.0f;
    test_simulation_press(&platform, BUTTON_TYPE_SPACE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(actor->velocity[AXIS_Z][judge_row], 1.0f, 0.0f);

    // Releasing Tab lets go of the judge, which stops where it stands.
    test_simulation_press(&platform, BUTTON_TYPE_TAB, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_S32(control.actor_id, population->judge_id);

    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_S32(control.actor_id, TABLE_KEY_NONE);
    TEST_CHECK_F32(actor->move_speed[judge_row], 0.0f, 0.0f);
    TEST_CHECK_F32(actor->velocity[AXIS_Z][judge_row], 0.0f, 0.0f);

    // The free camera flies: E raises it, W moves it along its forward
    // direction, and the judge is no longer driven.
    before = control.position;
    test_simulation_press(&platform, BUTTON_TYPE_E, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(control.position.z - before.z, DEBUG_CONTROL_SPEED * FIXED_FRAME_TIME_32, 1e-5f);
    TEST_CHECK_F32(control.position.x, before.x, 1e-5f);

    before = control.position;
    test_simulation_press(&platform, BUTTON_TYPE_W, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_F32(
        vec3_length(vec3_subtract(control.position, before)),
        DEBUG_CONTROL_SPEED * FIXED_FRAME_TIME_32, 1e-4f
    );
    TEST_CHECK_F32(
        vec3_dot(vec3_normalize(vec3_subtract(control.position, before)), quaternion_get_forward(control.orientation)),
        1.0f, 1e-4f
    );
    TEST_CHECK_F32(actor->velocity[AXIS_Y][judge_row], 0.0f, 0.0f);

    test_simulation_press(&platform, BUTTON_TYPE_Q, BUTTON_TYPE_NONE);
    platform.pointer_delta_x = 10.0;
    before = control.rotation;
    control_update(&control, &platform, population);

    TEST_CHECK_F32(control.rotation.z, before.z - 10.0f * CAMERA_SENSITIVITY_X, 1e-4f);

    // Tab again takes the judge back and restores its speed.
    test_simulation_press(&platform, BUTTON_TYPE_TAB, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);
    test_simulation_press(&platform, BUTTON_TYPE_NONE, BUTTON_TYPE_NONE);
    control_update(&control, &platform, population);

    TEST_CHECK_S32(control.actor_id, population->judge_id);
    TEST_CHECK_F32(actor->move_speed[judge_row], JUDGE_DEFAULT_MOVE_SPEED, 0.0f);
    TEST_CHECK_F32(control.position.z, actor->position[AXIS_Z][judge_row] + 0.7f, 0.0f);
}

// Documented in tests/test.h.
void
test_simulation_run(void)
{
    static Population population;
    World* world = test_get_world();

    memset(&population, 0, sizeof(population));
    population_init(&population);

    test_simulation_population(&population);
    test_simulation_physics();
    test_simulation_decisions(world);
    test_simulation_schedule(world, &population);

    population_quit(&population);

    memset(&population, 0, sizeof(population));
    population_init(&population);

    test_simulation_control(&population);

    population_quit(&population);
}
