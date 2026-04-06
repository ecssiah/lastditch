#ifndef CORE_H
#define CORE_H

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "jsk.h"

#define EPSILON 1e-5f

typedef enum Axis Axis;
enum Axis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_COUNT,
};

typedef struct IntRect IntRect;
struct IntRect
{
    ivec2 position;
    ivec2 size;
};

void int_rect_max(const IntRect *rect, ivec2 out_max);
void int_rect_min(const IntRect *rect, ivec2 out_min);

b32 int_rect_overlaps(const IntRect *rect_a, const IntRect *rect_b);
i32 int_rect_subtract(const IntRect *rect_a, const IntRect *rect_b, IntRect *out_array);

typedef struct FloatRect FloatRect;
struct FloatRect
{
    vec2 min;
    vec2 max;
};

typedef struct IntBounds IntBounds;
struct IntBounds
{
    ivec3 min;
    ivec3 max;
};

typedef struct FloatBounds FloatBounds;
struct FloatBounds
{
    vec3 min;
    vec3 max;
};

#endif
