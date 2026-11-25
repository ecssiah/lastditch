// ============================================================================
// File:    src/app/data/actor.c
// Layer:   app
// Purpose: Storage management of the actor and decision tables declared in
//          app/data/actor.h.
// ============================================================================

#include "app/data/actor.h"

#include <assert.h>
#include <string.h>

// ----------------------------------------------------------------------------
// Function: actor_table_reserve
// Purpose:  Ensures every column of the actor table can hold a given number
//           of rows.
// Params:   table        - table to grow
//           row_required - number of rows that must fit
// Returns:  nothing
// Reads:    table.row_capacity
// Writes:   table: column storage, row_capacity
// ----------------------------------------------------------------------------
static void
actor_table_reserve(ActorTable* table, const s32 row_required)
{
    const s32 capacity = table_grow_capacity(table->row_capacity, row_required);
    s32 axis;

    if (capacity == table->row_capacity)
    {
        return;
    }

    table->actor_id = (s32*)column_resize(table->actor_id, sizeof(s32), capacity);
    table->actor_type = (u8*)column_resize(table->actor_type, sizeof(u8), capacity);
    table->nation_type = (NationType*)column_resize(table->nation_type, sizeof(NationType), capacity);
    table->move_speed = (f32*)column_resize(table->move_speed, sizeof(f32), capacity);
    table->turn_speed = (f32*)column_resize(table->turn_speed, sizeof(f32), capacity);
    table->orientation = (Quaternion*)column_resize(table->orientation, sizeof(Quaternion), capacity);
    table->movement_yaw = (f32*)column_resize(table->movement_yaw, sizeof(f32), capacity);
    table->collision_enabled = (u8*)column_resize(table->collision_enabled, sizeof(u8), capacity);
    table->is_grounded = (u8*)column_resize(table->is_grounded, sizeof(u8), capacity);

    for (axis = 0; axis < AXIS_COUNT; ++axis)
    {
        table->position[axis] = (f32*)column_resize(table->position[axis], sizeof(f32), capacity);
        table->position_target[axis] = (f32*)column_resize(table->position_target[axis], sizeof(f32), capacity);
        table->rotation_target[axis] = (f32*)column_resize(table->rotation_target[axis], sizeof(f32), capacity);
        table->velocity[axis] = (f32*)column_resize(table->velocity[axis], sizeof(f32), capacity);
        table->collider_radius[axis] = (f32*)column_resize(table->collider_radius[axis], sizeof(f32), capacity);
    }

    table->row_capacity = capacity;
}

// Documented in src/app/data/actor.h.
s32
actor_table_insert(ActorTable* table, const ActorRecord* record)
{
    const s32 row = table->row_count;

    assert(record->actor_id >= 0);
    assert(row == 0 || table->actor_id[row - 1] < record->actor_id);

    actor_table_reserve(table, row + 1);

    table->actor_id[row] = record->actor_id;
    table->actor_type[row] = (u8)record->actor_type;
    table->nation_type[row] = record->nation_type;
    table->move_speed[row] = record->move_speed;
    table->turn_speed[row] = record->turn_speed;
    table->orientation[row] = record->orientation;
    table->movement_yaw[row] = record->movement_yaw;
    table->collision_enabled[row] = record->collision_enabled ? 1 : 0;
    table->is_grounded[row] = record->is_grounded ? 1 : 0;

    table->position[AXIS_X][row] = record->position.x;
    table->position[AXIS_Y][row] = record->position.y;
    table->position[AXIS_Z][row] = record->position.z;

    table->position_target[AXIS_X][row] = record->position_target.x;
    table->position_target[AXIS_Y][row] = record->position_target.y;
    table->position_target[AXIS_Z][row] = record->position_target.z;

    table->rotation_target[AXIS_X][row] = record->rotation_target.x;
    table->rotation_target[AXIS_Y][row] = record->rotation_target.y;
    table->rotation_target[AXIS_Z][row] = record->rotation_target.z;

    table->velocity[AXIS_X][row] = record->velocity.x;
    table->velocity[AXIS_Y][row] = record->velocity.y;
    table->velocity[AXIS_Z][row] = record->velocity.z;

    table->collider_radius[AXIS_X][row] = record->collider_radius.x;
    table->collider_radius[AXIS_Y][row] = record->collider_radius.y;
    table->collider_radius[AXIS_Z][row] = record->collider_radius.z;

    table->row_count = row + 1;

    return row;
}

// Documented in src/app/data/actor.h.
s32
actor_table_find_row(const ActorTable* table, const s32 actor_id)
{
    return key_column_find_row(table->actor_id, table->row_count, actor_id);
}

// Documented in src/app/data/actor.h.
Vec3
actor_table_get_position(const ActorTable* table, const s32 row)
{
    assert(row >= 0 && row < table->row_count);

    return vec3_make(
        table->position[AXIS_X][row],
        table->position[AXIS_Y][row],
        table->position[AXIS_Z][row]
    );
}

// Documented in src/app/data/actor.h.
Vec3
actor_table_get_rotation_target(const ActorTable* table, const s32 row)
{
    assert(row >= 0 && row < table->row_count);

    return vec3_make(
        table->rotation_target[AXIS_X][row],
        table->rotation_target[AXIS_Y][row],
        table->rotation_target[AXIS_Z][row]
    );
}

// Documented in src/app/data/actor.h.
void
actor_table_free(ActorTable* table)
{
    s32 axis;

    column_free(table->actor_id);
    column_free(table->actor_type);
    column_free(table->nation_type);
    column_free(table->move_speed);
    column_free(table->turn_speed);
    column_free(table->orientation);
    column_free(table->movement_yaw);
    column_free(table->collision_enabled);
    column_free(table->is_grounded);

    for (axis = 0; axis < AXIS_COUNT; ++axis)
    {
        column_free(table->position[axis]);
        column_free(table->position_target[axis]);
        column_free(table->rotation_target[axis]);
        column_free(table->velocity[axis]);
        column_free(table->collider_radius[axis]);
    }

    memset(table, 0, sizeof(*table));
}

// Documented in src/app/data/actor.h.
s32
decision_table_insert(DecisionTable* table, const s32 actor_id, const s32 decision_timer)
{
    const s32 row = table->row_count;
    const s32 capacity = table_grow_capacity(table->row_capacity, row + 1);

    assert(row == 0 || table->actor_id[row - 1] < actor_id);

    if (capacity != table->row_capacity)
    {
        table->actor_id = (s32*)column_resize(table->actor_id, sizeof(s32), capacity);
        table->decision_timer = (s32*)column_resize(table->decision_timer, sizeof(s32), capacity);
        table->row_capacity = capacity;
    }

    table->actor_id[row] = actor_id;
    table->decision_timer[row] = decision_timer;

    table->row_count = row + 1;

    return row;
}

// Documented in src/app/data/actor.h.
void
decision_table_free(DecisionTable* table)
{
    column_free(table->actor_id);
    column_free(table->decision_timer);

    memset(table, 0, sizeof(*table));
}
