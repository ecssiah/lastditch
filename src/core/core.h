#ifndef CORE_H
#define CORE_H

#include <cglm/cglm.h>
#include <glad/glad.h>

#include "justsky.h"

#define EPSILON 1e-5f

typedef enum Axis Axis;
enum Axis
{
    AXIS_X,
    AXIS_Y,
    AXIS_Z,

    AXIS_COUNT,
};

typedef struct IRect IRect;
struct IRect
{
    ivec2 position;
    ivec2 size;
};

void irect_max(const IRect *rect, ivec2 out_max);
void irect_min(const IRect *rect, ivec2 out_min);

b32 irect_overlaps(const IRect *rect_left, const IRect *rect_right);
IRect irect_intersection(const IRect *rect_left, const IRect *rect_right);

u32 irect_subtract(const IRect *rect_left, const IRect *rect_right, IRect *out_array);

void irect_print(const IRect *rect);

typedef struct FRect FRect;
struct FRect
{
    vec2 min;
    vec2 max;
};

typedef struct IBounds IBounds;
struct IBounds
{
    ivec3 min;
    ivec3 max;
};

typedef struct FBounds FBounds;
struct FBounds
{
    vec3 min;
    vec3 max;
};

#endif
