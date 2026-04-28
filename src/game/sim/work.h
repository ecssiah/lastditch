#ifndef WORK_H
#define WORK_H

#include <cglm/cglm.h>

#include "justsky.h"

#include "game/sim/actor.h"
#include "game/sim/ids.h"
#include "game/sim/navigation.h"

#define ACT_MAX 1 << 12

typedef struct Sim Sim;

typedef struct WanderState WanderState;
struct WanderState
{
    u32 tick;
    u32 tick_limit;
};

typedef struct SeekState SeekState;
struct SeekState
{
    ivec3 target_position;
};

#define FOR_LIST_ACT_TYPE(DO)              \
    DO(ACT_TYPE_WANDER)                    \
    DO(ACT_TYPE_SEEK)                      \

enum ActType
{
    FOR_LIST_ACT_TYPE(DEFINE_LIST_ENUMERATION)

    ACT_TYPE_COUNT
};

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
    u32 active_count;
    ActID active_array[ACT_MAX];
    
    PoolID active_lookup[ACT_MAX];

    u32 free_count;    
    ActID free_array[ACT_MAX];
    
    Act act_array[ACT_MAX];
};

struct Work
{
    ActPool act_pool;
};

void wander_run(Sim *sim, Act *act, f32 delta_time);
void seek_run(Sim *sim, Act *act, f32 delta_time);

ActID work_add_act(Work *work, Actor *actor, ActType act_type, ActState act_state);

void work_init(Work *work);
void work_update(Work *work, Sim *sim, f32 delta_time);

#endif
