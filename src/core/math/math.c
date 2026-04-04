#include "math.h"

#include <math.h>

f32 math_lerp_to(f32 current, f32 target, f32 speed, f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > EPSILON)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    else
    {
        return target;
    }
}
