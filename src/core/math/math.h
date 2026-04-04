#ifndef MATH_H
#define MATH_H

#include "jsk.h"

#define EPSILON 1e-5f

f32 math_lerp_to(f32 current, f32 target, f32 speed, f32 delta_time);

#endif
