// ============================================================================
// File:    src/app/data/actor_reference.h
// Layer:   app
// Purpose: Nation type reference tables.
// ============================================================================

#ifndef ACTOR_REFERENCE_H
#define ACTOR_REFERENCE_H

#include "core/types.h"

// ----------------------------------------------------------------------------
// Enum:    NationTypeId
// Purpose: Keys of the nation type reference tables.
// ----------------------------------------------------------------------------
typedef enum NationTypeId
{
#define NATION(id, name, texture) NATION_TYPE_ ## id,
#include "nation_types.def"
#undef NATION
    // Number of nations.
    NATION_TYPE_COUNT
} NationTypeId;

// ----------------------------------------------------------------------------
// Type:    NationType
// Purpose: Storage type of a NationTypeId in tables.
// ----------------------------------------------------------------------------
typedef u8 NationType;
typedef char NationTypeCapacityCheck[NATION_TYPE_COUNT <= 256 ? 1 : -1];

// ----------------------------------------------------------------------------
// Reference: NATION_TYPE_NAME
// Purpose:   Name of each nation.
// Key:       NationTypeId
// Order:     rows in NationTypeId order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const char *const NATION_TYPE_NAME[NATION_TYPE_COUNT];

// ----------------------------------------------------------------------------
// Reference: NATION_TEXTURE_NAME
// Purpose:   Base name of the texture file, under assets/textures/actor,
//            worn by the actors of each nation.
// Key:       NationTypeId
// Order:     rows in NationTypeId order
// Lifetime:  compiled in, read-only
// ----------------------------------------------------------------------------
extern const char *const NATION_TEXTURE_NAME[NATION_TYPE_COUNT];

#endif
