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
    // Requested; the search has not finished.
    PATH_STATUS_PENDING,
    // The search finished without finding a route.
    PATH_STATUS_UNREACHABLE,
    // The request was withdrawn.
    PATH_STATUS_CANCELLED,
    // A route was found; its waypoints are available.
    PATH_STATUS_READY
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
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // Issues path_id values.
    Sequence id_sequence;
    // [PK] Key of the path.
    s32 *path_id;
    // PathStatus of the request.
    u8 *status;
    // Position, within the path's waypoints, of the next one to reach.
    s32 *waypoint_index;
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
    // Number of rows stored.
    s32 row_count;
    // Number of rows the columns can hold.
    s32 row_capacity;
    // [PK] [FK -> PathTable.path_id] Path the waypoint belongs to.
    s32 *path_id;
    // [PK] Position of the waypoint along its path, starting at 0.
    s32 *sequence_number;
    // Cell of the waypoint, one column per axis.
    s32 *coordinate[AXIS_COUNT];
} WaypointTable;

#endif
