// ============================================================================
// File:    src/app/data/actor_reference.c
// Layer:   app
// Purpose: Nation names and actor textures in NationTypeId order.
// ============================================================================

#include "app/data/actor_reference.h"

const char* const NATION_TYPE_NAME[NATION_TYPE_COUNT] =
{
#define NATION(id, name, texture) #name,
#include "nation_types.def"
#undef NATION
};

const char* const NATION_TEXTURE_NAME[NATION_TYPE_COUNT] =
{
#define NATION(id, name, texture) #texture,
#include "nation_types.def"
#undef NATION
};
