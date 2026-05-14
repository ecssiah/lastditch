#pragma once

#include "core/types.h"
#include "app/actor.h"

constexpr i32 act_max_count = 1 << 12;

struct Population;

struct WanderState
{
    i32 tick;
    i32 tick_limit;
};

struct SeekState
{
    IVec3 target_position;
};

#define FOR_LIST_ACT_TYPE(DO)                                                       \
    DO(wander)                                                                      \
    DO(seek)                                                                        \

enum ActType
{
    FOR_LIST_ACT_TYPE(DEFINE_ENUM_VARIANTS)
};

constexpr i32 act_type_count = FOR_LIST_ACT_TYPE(DEFINE_ENUM_COUNT);

union ActState
{
    WanderState wander;
    SeekState seek;
};

struct Act
{
    i32 actor_id;

    ActType act_type;
    ActState act_state;
};

struct ActPool
{
    i32 active_count;
    i32 active_array[act_max_count];

    i32 active_lookup[act_max_count];

    i32 free_count;
    i32 free_array[act_max_count];

    Act act_array[act_max_count];
};

struct Work
{
    ActPool act_pool;
};

void wander_run(Population& population, Act& act, f32 delta_time);
void seek_run(Population& population, Act& act, f32 delta_time);

i32 work_add_act(Work& work, Actor& actor, ActType act_type, ActState act_state);

void work_init(Work& work);
void work_update(Population& population, Work& work, f32 delta_time);
