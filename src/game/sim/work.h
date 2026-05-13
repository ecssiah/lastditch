#pragma once

#include "core/types.h"
#include "game/sim/actor.h"
#include "game/sim/ids.h"

constexpr i32 ACT_MAX = 1 << 12;

struct Sim;

struct WanderState
{
    i32 tick;
    i32 tick_limit;
};

struct SeekState
{
    ivec3 target_position;
};

#define FOR_LIST_ACT_TYPE(DO)                                                    \
    DO(wander)                                                                   \
    DO(seek)

enum ActType
{
    FOR_LIST_ACT_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 ACT_TYPE_COUNT = FOR_LIST_ACT_TYPE(DEFINE_ENUM_COUNT);

union ActState
{
    WanderState wander;
    SeekState seek;
};

struct Act
{
    ActorID actor_id;

    ActType act_type;
    ActState act_state;
};

struct ActPool
{
    i32 active_count;
    ActID active_array[ACT_MAX];

    PoolID active_lookup[ACT_MAX];

    i32 free_count;
    ActID free_array[ACT_MAX];

    Act act_array[ACT_MAX];
};

struct Work
{
    ActPool act_pool;
};

void wander_run(Sim& sim, Act& act, f32 delta_time);
void seek_run(Sim& sim, Act& act, f32 delta_time);

ActID work_add_act(Work& work, Actor& actor, ActType act_type, ActState act_state);

void work_init(Work& work);
void work_update(Work& work, Sim& sim, f32 delta_time);
