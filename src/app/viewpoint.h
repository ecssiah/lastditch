#pragma once

#include "core/geometry.h"

class Viewpoint
{
public:
    Vec3 position {};
    Vec3 rotation {};

    Mat4 projection_matrix {};
    Mat4 view_matrix {};
};
