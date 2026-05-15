#pragma once

#include "core/geometry.h"

struct Viewpoint
{
    Vec3 position;
    Vec3 rotation;

    Mat4 projection_matrix;
    Mat4 view_matrix;
};
