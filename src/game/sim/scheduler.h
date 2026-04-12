#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <cglm/cglm.h>

#include "justsky.h"

#include "game/sim/actor.h"
#include "game/sim/navigation.h"

#define BEHAVIOR_MAX 1 << 12

typedef struct WanderState WanderState;
struct WanderState
{
    u32 clock;
    u32 timer;
};

void wander_run(Actor *actor, f32 delta_time);

typedef struct SeekState SeekState;
struct SeekState
{
    Graph *graph;
    ivec3 target_position;
};

void seek_run(Actor *actor, f32 delta_time);

typedef void (*BehaviorFn)(Actor *actor, f32 delta_time);

typedef u32 BehaviorID;

typedef struct Behavior Behavior;
struct Behavior
{
    BehaviorFn behavior_fn;
    void *data;

    ActorID actor_id;
};

typedef struct BehaviorPool BehaviorPool;
struct BehaviorPool
{
    u32 active_count;
    u32 active_array[BEHAVIOR_MAX];
    u32 active_lookup[BEHAVIOR_MAX];

    u32 free_count;    
    u32 free_array[BEHAVIOR_MAX];
    
    Behavior behavior_array[BEHAVIOR_MAX];
};

typedef struct Scheduler Scheduler;
struct Scheduler
{
    BehaviorPool behavior_pool;
};

void scheduler_init(Scheduler *scheduler);
void scheduler_update(Scheduler *scheduler, f32 delta_time);

#endif
