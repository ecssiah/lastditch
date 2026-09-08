#pragma once

#include "core/geometry.h"
#include "core/types.h"

struct Collider
{
    b32 collision_enabled { true };

    Vec3 radius {};
};
