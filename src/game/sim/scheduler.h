#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cglm/cglm.h>

#include "justsky.h"

#include "game/sim/actor.h"
#include "game/sim/ids.h"
#include "game/sim/navigation.h"

#define BEHAVIOR_MAX 1 << 12

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
    Graph *graph;
    ivec3 target_position;
};

#define FOR_LIST_BEHAVIOR_TYPE(DO)              \
    DO(BEHAVIOR_TYPE_WANDER)                    \
    DO(BEHAVIOR_TYPE_SEEK)                      \

typedef enum BehaviorType BehaviorType;
enum BehaviorType
{
    FOR_LIST_BEHAVIOR_TYPE(DEFINE_LIST_ENUMERATION)

    BEHAVIOR_TYPE_COUNT
};

typedef union BehaviorState BehaviorState;
union BehaviorState
{
    WanderState wander;
    SeekState seek;
};

typedef struct Behavior Behavior;
struct Behavior
{
    ActorID actor_id;

    BehaviorType behavior_type;
    BehaviorState behavior_state;
};

typedef struct BehaviorPool BehaviorPool;
struct BehaviorPool
{
    u32 active_count;
    BehaviorID active_array[BEHAVIOR_MAX];
    
    PoolID active_lookup[BEHAVIOR_MAX];

    u32 free_count;    
    BehaviorID free_array[BEHAVIOR_MAX];
    
    Behavior behavior_array[BEHAVIOR_MAX];
};

typedef struct Scheduler Scheduler;
struct Scheduler
{
    BehaviorPool behavior_pool;
};

void wander_run(Sim *sim, Behavior *behavior, f32 delta_time);
void seek_run(Sim *sim, Behavior *behavior, f32 delta_time);

BehaviorID scheduler_add_behavior(Scheduler *scheduler, Actor *actor, BehaviorType behavior_type, BehaviorState behavior_state);

void scheduler_init(Scheduler *scheduler);
void scheduler_update(Scheduler *scheduler, Sim *sim, f32 delta_time);

#endif
