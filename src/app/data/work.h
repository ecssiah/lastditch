// ============================================================================
// File:    src/app/data/work.h
// Layer:   app
// Purpose: Row type of the work schedule: the table that decides which
//          simulation systems run on which simulation step.
// ============================================================================

#ifndef APP_DATA_WORK_H
#define APP_DATA_WORK_H

#include "core/types.h"

struct World;
struct Population;

// ----------------------------------------------------------------------------
// Type:    WorkFunction
// Purpose: Signature of a simulation system that can be scheduled. A system
//          is a transform over tables: it receives the world and population
//          databases and updates rows in place.
// ----------------------------------------------------------------------------
typedef void (*WorkFunction)(struct World* world, struct Population* population);

// ----------------------------------------------------------------------------
// Record:  WorkRecord
// Purpose: Row type of WORK_TABLE. The work runs on every simulation step
//          for which (step + phase) is a multiple of frequency.
// ----------------------------------------------------------------------------
typedef struct WorkRecord
{
    s32 frequency;                  // Run once every this many simulation steps; at least 1.
    s32 phase;                      // Offset added to the step number before testing the frequency.
    WorkFunction work_function;     // System to run.
} WorkRecord;

#endif
