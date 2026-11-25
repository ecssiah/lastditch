// ============================================================================
// File:    src/app/data/path.h
// Layer:   app
// Purpose: Schema of the path tables reserved for navigation. Path finding
//          is not implemented yet: nothing creates or reads these tables,
//          they record the intended data layout.
// ============================================================================

#ifndef APP_DATA_PATH_H
#define APP_DATA_PATH_H

#include "core/table.h"
#include "core/types.h"
#include "core/geometry.h"

// ----------------------------------------------------------------------------
// Enum:    PathStatus
// Purpose: Domain of PathTable.status: the life cycle of a path request.
// ----------------------------------------------------------------------------
typedef enum PathStatus
{
    PATH_STATUS_PENDING,            // Requested; the search has not finished.
    PATH_STATUS_UNREACHABLE,        // The search finished without finding a route.
    PATH_STATUS_CANCELLED,          // The request was withdrawn.
    PATH_STATUS_READY               // A route was found; its waypoints are available.
} PathStatus;

// ----------------------------------------------------------------------------
// Table:    PathTable
// Purpose:  One row per path request.
// Key:      path_id, a surrogate key issued by id_sequence
// Order:    clustered on path_id, ascending
// Lifetime: reserved; not yet created by any system
// ----------------------------------------------------------------------------
typedef struct PathTable
{
    s32 row_count;                  // Number of rows stored.
    s32 row_capacity;               // Number of rows the columns can hold.
    Sequence id_sequence;           // Issues path_id values.
    s32* path_id;                   // [PK] Key of the path.
    u8* status;                     // PathStatus of the request.
    s32* waypoint_index;            // Position, within the path's waypoints, of the next one to reach.
} PathTable;

// ----------------------------------------------------------------------------
// Table:    WaypointTable
// Purpose:  The cells a path leads through, one row per waypoint.
// Key:      (path_id, sequence_number)
// Order:    clustered on path_id, then sequence_number, ascending
// Lifetime: reserved; not yet created by any system
// ----------------------------------------------------------------------------
typedef struct WaypointTable
{
    s32 row_count;                  // Number of rows stored.
    s32 row_capacity;               // Number of rows the columns can hold.
    s32* path_id;                   // [PK] [FK -> PathTable.path_id] Path the waypoint belongs to.
    s32* sequence_number;           // [PK] Position of the waypoint along its path, starting at 0.
    s32* coordinate[AXIS_COUNT];    // Cell of the waypoint, one column per axis.
} WaypointTable;

#endif
