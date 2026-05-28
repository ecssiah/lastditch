#pragma once

#include <vector>

#include "core/macros.h"
#include "core/types.h"

constexpr f32 epsilon = 1e-5f;
constexpr f32 epsilon_squared = 1e-10f;

#define FOR_LIST_AXIS(DO)                                   \
    DO(X)                                                   \
    DO(Y)                                                   \
    DO(Z)                                                   \

enum class Axis : u8
{
    FOR_LIST_AXIS(DEFINE_ENUM_VARIANTS)
};

constexpr s32 axis_count = FOR_LIST_AXIS(DEFINE_ENUM_COUNT);

struct Vec2
{
    f32 x;
    f32 y;

    constexpr f32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const f32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }
};

struct Vec3
{
    f32 x;
    f32 y;
    f32 z;
    
    constexpr f32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const f32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }
};

struct IVec2
{
    s32 x;
    s32 y;
    
    constexpr s32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const s32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }
};

struct IVec3
{
    s32 x;
    s32 y;
    s32 z;
    
    constexpr s32&
    operator[](const s32 index)
    {
        return (&x)[index];
    }

    constexpr const s32&
    operator[](const s32 index) const
    {
        return (&x)[index];
    }
};

struct Mat4
{
    f32 entry_array[4][4];

    constexpr f32*
    operator[](const s32 index)
    {
        return entry_array[index];
    }

    constexpr const f32*
    operator[](const s32 index) const
    {
        return entry_array[index];
    }
};

struct IBounds2
{
    IVec2 min;
    IVec2 max;
};

struct IBounds3
{
    IVec3 min;
    IVec3 max;
};

struct Bounds2
{
    Vec2 min;
    Vec2 max;
};

struct Bounds3
{
    Vec3 min;
    Vec3 max;
};

constexpr Vec3 unit_x = {1.0f, 0.0f, 0.0f};
constexpr Vec3 unit_y = {0.0f, 1.0f, 0.0f};
constexpr Vec3 unit_z = {0.0f, 0.0f, 1.0f};

constexpr s32 
min_s32(const s32 a, const s32 b)
{
    return a < b ? a : b;
}

constexpr s32 
max_s32(const s32 a, const s32 b)
{
    return a > b ? a : b;
}

constexpr f32 
min_f32(const f32 a, const f32 b)
{
    return a < b ? a : b;
}

constexpr f32 
max_f32(const f32 a, const f32 b)
{
    return a > b ? a : b;
}

constexpr s32
clamp_s32(const s32 a, const s32 min, const s32 max)
{
    return a < min ? min : a > max ? max : a;
}

constexpr f32
clamp_f32(const f32 a, const f32 min, const f32 max)
{
    return a < min ? min : a > max ? max : a;
}

constexpr f32
to_radians(const f32 degrees)
{
    return degrees * 0.01745329251994329576923690768489f;
}

constexpr f32
to_degrees(const f32 radians)
{
    return radians * 57.295779513082323f;
}

constexpr Vec2 
vec2_broadcast(const f32 scalar)
{
    return {scalar, scalar};
}

constexpr Vec3 
vec3_broadcast(const f32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr IVec2 
ivec2_broadcast(const s32 scalar)
{
    return {scalar, scalar};
}

constexpr IVec3 
ivec3_broadcast(const s32 scalar)
{
    return {scalar, scalar, scalar};
}

constexpr Mat4
mat4_diagonal(const f32 scalar)
{
    return {
        {
            {scalar, 0.0f, 0.0f, 0.0f },
            {0.0f, scalar, 0.0f, 0.0f },
            {0.0f, 0.0f, scalar, 0.0f },
            {0.0f, 0.0f, 0.0f, scalar },
        }
    };
}

Vec2 operator+(const Vec2& a, const Vec2& b);
Vec3 operator+(const Vec3& a, const Vec3& b);
IVec2 operator+(const IVec2& a, const IVec2& b);
IVec3 operator+(const IVec3& a, const IVec3& b);

Vec2 operator-(const Vec2& a, const Vec2& b);
Vec3 operator-(const Vec3& a, const Vec3& b);
IVec2 operator-(const IVec2& a, const IVec2& b);
IVec3 operator-(const IVec3& a, const IVec3& b);

Vec2 operator*(f32 scalar, const Vec2& a);
Vec3 operator*(f32 scalar, const Vec3& a);
IVec2 operator*(s32 scalar, const IVec2& a);
IVec3 operator*(s32 scalar, const IVec3& a);

Vec2 ivec2_to_vec2(const IVec2& a);
Vec3 ivec3_to_vec3(const IVec3& a);

Vec3 vec3_max(const Vec3& a, const Vec3& b);
Vec3 vec3_min(const Vec3& a, const Vec3& b);

f32 vec3_length(const Vec3& a);
f32 vec2_length(const Vec2& a);

f32 vec3_length_squared(const Vec3& a);
f32 vec2_length_squared(const Vec2& a);

Vec2 vec2_normalize(const Vec2& a);
Vec3 vec3_normalize(const Vec3& a);

f32 vec2_dot(const Vec2& a, const Vec2& b);
f32 vec3_dot(const Vec3& a, const Vec3& b);

Vec3 vec3_cross(const Vec3& a, const Vec3& b);

Mat4 operator*(const Mat4& a, const Mat4& b);

Mat4 mat4_translate(const Mat4& a, const Vec3& translation);
Mat4 mat4_rotate(const Mat4& a, f32 angle, const Vec3& axis);

Mat4 get_view_matrix(const Vec3& position, const Vec3& rotation);
Mat4 get_orthographic_matrix(const Vec2& min, const Vec2& max, f32 near, f32 far);
Mat4 get_projection_matrix(f32 fov_y, f32 aspect, f32 near, f32 far);

Mat4 get_look_at_matrix(const Vec3& position, const Vec3& target, const Vec3& up);

f32 interpolate_to(f32 current, f32 target, f32 speed, f32 delta_time);

Vec3 get_forward(const Vec3& rotation);
Vec3 get_right(const Vec3& rotation);
Vec3 get_up(const Vec3& rotation);

IVec2 ibounds2_position(const IBounds2& a);
IVec2 ibounds2_size(const IBounds2& a);

b32 ibounds2_overlaps(const IBounds2& a, const IBounds2& b);
IBounds2 ibounds2_intersection(const IBounds2& a, const IBounds2& b);

std::vector<IBounds2> ibounds2_subtract(const IBounds2& a, const IBounds2& b);
