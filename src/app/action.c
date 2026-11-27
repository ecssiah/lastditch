// ============================================================================
// File:    src/app/action.c
// Layer:   app
// Purpose: Implementation of the simulation driver, the work schedule and
//          the work functions declared in app/action.h.
// ============================================================================

#include "app/action.h"

#include <assert.h>

#include "app/navigation.h"
#include "app/physics.h"
#include "core/geometry.h"

const WorkRecord WORK_TABLE[WORK_COUNT] =
{
    // frequency, phase, system
    { 4, 0, work_decide },
    { 1, 0, work_orient },
    { 1, 0, work_move }
};

// Documented in src/app/action.h.
void
work_decide(World *world,
            Population *population)
{
    DecisionTable *decision = &population->decision;
    ActorTable *actor = &population->actor;
    s32 decision_row;

    (void)world;

    for (decision_row = 0; decision_row < decision->row_count; ++decision_row)
    {
        const s32 row = actor_table_find_row(actor, decision->actor_id[decision_row]);
        f32 distance_to_target;

        assert(row != TABLE_ROW_NONE);

        if (decision->decision_timer[decision_row] > 0)
        {
            --decision->decision_timer[decision_row];
            continue;
        }

        decision->decision_timer[decision_row] = random_uniform_s32(&population->random, 10, 50);

        distance_to_target = angle_get_shortest_delta(
            actor->movement_yaw[row],
            actor->rotation_target[AXIS_Z][row]
        );

        if (f32_abs(distance_to_target) < 1.0f)
        {
            const int should_act = random_uniform_s32(&population->random, 0, 1) == 1;

            if (should_act)
            {
                Vec2 direction;

                actor->rotation_target[AXIS_Z][row] = f32_clamp(
                    actor->rotation_target[AXIS_Z][row] + random_uniform_f32(&population->random, -120.0f, 120.0f),
                    0.0f,
                    360.0f
                );

                direction = direction_from_angle(actor->rotation_target[AXIS_Z][row]);

                actor->velocity[AXIS_X][row] = direction.x * actor->move_speed[row];
                actor->velocity[AXIS_Y][row] = direction.y * actor->move_speed[row];
            }
            else
            {
                actor->velocity[AXIS_X][row] = 0.0f;
                actor->velocity[AXIS_Y][row] = 0.0f;
            }
        }
    }
}

// Documented in src/app/action.h.
void
work_orient(World *world,
            Population *population)
{
    ActorTable *actor = &population->actor;
    s32 row;

    (void)world;

    for (row = 0; row < actor->row_count; ++row)
    {
        const f32 alpha = 1.0f - f32_exp(-actor->turn_speed[row] * FIXED_FRAME_TIME_32);

        actor->movement_yaw[row] = angle_interpolate_to(
            actor->movement_yaw[row],
            actor->rotation_target[AXIS_Z][row],
            actor->turn_speed[row],
            FIXED_FRAME_TIME_32
        );

        actor->orientation[row] = quaternion_spherical_interpolation_to(
            actor->orientation[row],
            quaternion_from_euler_degrees(
                actor->rotation_target[AXIS_X][row],
                actor->rotation_target[AXIS_Y][row],
                actor->rotation_target[AXIS_Z][row]
            ),
            alpha
        );
    }
}

// Documented in src/app/action.h.
void
work_move(World *world,
          Population *population)
{
    ActorTable *actor = &population->actor;
    s32 row;

    for (row = 0; row < actor->row_count; ++row)
    {
        physics_update_actor(world, actor, row);
    }
}

// Documented in src/app/action.h.
void
action_init(Action *action,
            World *world)
{
    random_seed(&action->random, WORK_SEED);

    action->tick_count = 0;

    navigation_init(world);
}

// Documented in src/app/action.h.
void
action_update(Action *action,
              World *world,
              Population *population)
{
    s32 work_row;

    ++action->tick_count;

    for (work_row = 0; work_row < WORK_COUNT; ++work_row)
    {
        const WorkRecord *work = &WORK_TABLE[work_row];

        if (action_is_due(action, work))
        {
            work->work_function(world, population);
        }
    }
}

// Documented in src/app/action.h.
int
action_is_due(const Action *action,
              const WorkRecord *work)
{
    return (action->tick_count + (u64)work->phase) % (u64)work->frequency == 0;
}
