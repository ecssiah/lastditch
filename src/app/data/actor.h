// ============================================================================
// File:    src/app/data/actor.h
// Layer:   app
// Purpose: The actor table (everything that moves through the world) and
//          the decision table (the actors that steer themselves).
// ============================================================================

#ifndef APP_DATA_ACTOR_H
#define APP_DATA_ACTOR_H

#include "app/data/actor_reference.h"

#include "core/geometry.h"
#include "core/table.h"
#include "core/types.h"

// ----------------------------------------------------------------------------
// Enum:    ActorType
// Purpose: Domain of ActorTable.actor_type.
// ----------------------------------------------------------------------------
typedef enum ActorType
{
    // No role assigned.
    ACTOR_TYPE_NONE,
    // The actor the player embodies.
    ACTOR_TYPE_JUDGE,
    // A member of a nation that acts on its own.
    ACTOR_TYPE_AGENT,
    // Number of actor types.
    ACTOR_TYPE_COUNT
} ActorType;

// ----------------------------------------------------------------------------
// Record:  ActorRecord
// Purpose: One row of the actor table as a value, used to insert actors.
// ----------------------------------------------------------------------------
typedef struct ActorRecord
{
    // Primary key of the row.
    s32 actor_id;
    // Role of the actor.
    ActorType actor_type;
    // Nation the actor belongs to.
    NationType nation_type;
    // Horizontal speed when moving, in cells per second.
    f32 move_speed;
    // Rate at which the actor turns towards rotation_target.
    f32 turn_speed;
    // Center of the actor's collider, in world space.
    Vec3 position;
    // Position the actor is heading for.
    Vec3 position_target;
    // Current orientation, smoothed towards rotation_target.
    Quaternion orientation;
    // Wanted Euler rotation in degrees: x pitch, y roll, z yaw.
    Vec3 rotation_target;
    // Current heading in degrees, smoothed towards rotation_target.z.
    f32 movement_yaw;
    // Velocity in cells per second.
    Vec3 velocity;
    // Whether the actor collides with blocks.
    int collision_enabled;
    // Half extents of the actor's box collider.
    Vec3 collider_radius;
    // Whether the actor stood on a block after the last physics step.
    int is_grounded;
} ActorRecord;

// ----------------------------------------------------------------------------
// Table:    ActorTable
// Purpose:  State of every actor. The table is column-oriented so that each
//           system touches only the attributes it works on: physics reads
//           and writes position, velocity and the collider columns, one axis
//           at a time; rendering reads nation_type, position and
//           movement_yaw; and so on.
// Key:      actor_id, a surrogate key issued by id_sequence
// Order:    clustered on actor_id, ascending; rows are only ever appended
// Lifetime: filled by population_init; updated every simulation step by
//           the work functions and by control; released by actor_table_free
// ----------------------------------------------------------------------------
typedef struct ActorTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Issues actor_id values.
    Sequence id_sequence;
    // [PK] Key of the actor.
    s32 *actor_id;
    // ActorType of the actor.x
    u8 *actor_type;
    // [FK -> NATION_TABLE.nation_type] Nation the actor belongs to.
    NationType *nation_type;
    // Horizontal speed when moving, in cells per second.
    f32 *move_speed;
    // Rate at which the actor turns towards its rotation target.
    f32 *turn_speed;
    // Center of the collider in world space, one column per axis.
    f32 *position[AXIS_COUNT];
    // Position the actor is heading for, one column per axis.
    f32 *position_target[AXIS_COUNT];
    // Current orientation, smoothed towards the rotation target.
    Quaternion *orientation;
    // Wanted Euler rotation in degrees, one column per axis: pitch, roll, yaw.
    f32 *rotation_target[AXIS_COUNT];
    // Current heading in degrees, smoothed towards rotation_target[AXIS_Z].
    f32 *movement_yaw;
    // Velocity in cells per second, one column per axis.
    f32 *velocity[AXIS_COUNT];
    // Non-zero when the actor collides with blocks.
    u8 *collision_enabled;
    // Half extents of the box collider, one column per axis.
    f32 *collider_radius[AXIS_COUNT];
    // [derived] Non-zero when the last physics step ended standing on a block.
    u8 *is_grounded;
} ActorTable;

// ----------------------------------------------------------------------------
// Table:    DecisionTable
// Purpose:  The actors that decide for themselves where to go, and the
//           countdown to their next decision. An actor steers itself exactly
//           when it has a row here, so the decision system needs no test of
//           actor type: the judge simply has no row.
// Key:      actor_id
// Order:    clustered on actor_id, ascending; rows are only ever appended
// Lifetime: filled by population_init; timers updated by the decision work
//           function; released by decision_table_free
// ----------------------------------------------------------------------------
typedef struct DecisionTable
{
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // [PK] [FK -> ActorTable.actor_id] Actor that steers itself.
    s32 *actor_id;
    // Decision steps left before the actor reconsiders; 0 means decide now.
    s32 *decision_timer;
} DecisionTable;

// ----------------------------------------------------------------------------
// Function: actor_table_insert
// Purpose:  Appends one actor to the table.
// Params:   table  - table to insert into
//           record - values of the new row; its actor_id must be greater
//                    than every key in the table
// Returns:  The row at which the actor was stored.
// Reads:    record
// Writes:   table: every column, row_count
// ----------------------------------------------------------------------------
s32 actor_table_insert(ActorTable *table, const ActorRecord *record);

// ----------------------------------------------------------------------------
// Function: actor_table_find_row
// Purpose:  Looks up an actor by key.
// Params:   table    - table to search
//           actor_id - key to look up
// Returns:  The row of the actor, or TABLE_ROW_NONE.
// Reads:    table.actor_id
// Writes:   nothing
// ----------------------------------------------------------------------------
s32 actor_table_find_row(const ActorTable *table, s32 actor_id);

// ----------------------------------------------------------------------------
// Function: actor_table_get_position
// Purpose:  Reads the three position columns of one row as a vector.
// Params:   table - table to read
//           row   - row to read, 0 <= row < row_count
// Returns:  The position of the actor.
// Reads:    table.position
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 actor_table_get_position(const ActorTable *table, s32 row);

// ----------------------------------------------------------------------------
// Function: actor_table_get_rotation_target
// Purpose:  Reads the three rotation target columns of one row as a vector.
// Params:   table - table to read
//           row   - row to read, 0 <= row < row_count
// Returns:  The wanted Euler rotation of the actor, in degrees.
// Reads:    table.rotation_target
// Writes:   nothing
// ----------------------------------------------------------------------------
Vec3 actor_table_get_rotation_target(const ActorTable *table, s32 row);

// ----------------------------------------------------------------------------
// Function: actor_table_free
// Purpose:  Releases the storage of the table and resets it to empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void actor_table_free(ActorTable *table);

// ----------------------------------------------------------------------------
// Function: decision_table_insert
// Purpose:  Appends a self-steering actor to the decision table.
// Params:   table          - table to insert into
//           actor_id       - key of the actor; must be greater than every
//                            key in the table
//           decision_timer - initial countdown to the first decision
// Returns:  The row at which the entry was stored.
// Reads:    arguments only
// Writes:   table: every column, row_count
// ----------------------------------------------------------------------------
s32 decision_table_insert(DecisionTable *table, s32 actor_id, s32 decision_timer);

// ----------------------------------------------------------------------------
// Function: decision_table_free
// Purpose:  Releases the storage of the table and resets it to empty.
// Params:   table - table to release
// Returns:  nothing
// Reads:    table
// Writes:   table
// ----------------------------------------------------------------------------
void decision_table_free(DecisionTable *table);

#endif
