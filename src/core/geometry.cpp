#include "geometry.h"

#include <cmath>

using namespace std;

Vec2::Vec2()
    :
    x{0.0f},
    y{0.0f}
{

}

Vec2::Vec2(const f32 s)
    :
    x{s},
    y{s}
{

}

Vec2::Vec2(f32 x_, f32 y_)
    :
    x{x_},
    y{y_}
{

}

Vec2::Vec2(const IVec2& ivec2)
    :
    x{static_cast<f32>(ivec2.x)},
    y{static_cast<f32>(ivec2.y)}
{

}

Vec3::Vec3()
    :
    x{0.0f},
    y{0.0f},
    z{0.0f}
{

}

Vec3::Vec3(const f32 s)
    :
    x{s},
    y{s},
    z{s}
{

}

Vec3::Vec3(const f32 x_, const f32 y_, const f32 z_)
    :
    x(x_),
    y(y_),
    z(z_)
{

}

Vec3::Vec3(const IVec3& ivec3)
    :
    x{static_cast<f32>(ivec3.x)},
    y{static_cast<f32>(ivec3.y)},
    z{static_cast<f32>(ivec3.z)}
{

}

IVec3::IVec3()
    :
    x{0},
    y{0},
    z{0}
{

}

IVec3::IVec3(const s32 s)
    :
    x{s},
    y{s},
    z{s}
{

}

IVec3::IVec3(const s32 x_, const s32 y_, const s32 z_)
    :
    x{x_},
    y{y_},
    z{z_}
{

}

Vec2
operator+(const Vec2& lhs, const Vec2& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
    };
}

Vec3 
operator+(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

IVec2 operator+(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
    };
}

IVec3 operator+(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

Vec2 
operator-(const Vec2& lhs, const Vec2& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
    };
}

Vec3 
operator-(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

IVec2 operator-(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
    };
}

IVec3 operator-(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

Vec2
operator*(const f32 lhs, const Vec2& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
    };
}

Vec3
operator*(const f32 lhs, const Vec3& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
        lhs * rhs.z,
    };
}

IVec2 operator*(const s32 lhs, const IVec2& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
    };
}

IVec3 operator*(const s32 lhs, const IVec3& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
        lhs * rhs.z,
    };
}

f32 
Vec2::get_length() const
{
    return sqrt(x * x + y * y);
}

f32 
Vec3::get_length() const
{
    return sqrt(x * x + y * y + z * z);
}

f32
Vec2::get_length_squared() const
{
    return x * x + y * y;
}

f32
Vec3::get_length_squared() const
{
    return x * x + y * y + z * z;
}

Vec3
max(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.x > rhs.x ? lhs.x : rhs.x,
        lhs.y > rhs.y ? lhs.y : rhs.y,
        lhs.z > rhs.z ? lhs.z : rhs.z,
    };
}

Vec3
min(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.x < rhs.x ? lhs.x : rhs.x,
        lhs.y < rhs.y ? lhs.y : rhs.y,
        lhs.z < rhs.z ? lhs.z : rhs.z,
    };
}

f32
dot(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

f32 
dot(const Vec3& lhs, const Vec3& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

Vec3 
cross(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.y * rhs.z - lhs.z * rhs.y,
        lhs.z * rhs.x - lhs.x * rhs.z,
        lhs.x * rhs.y - lhs.y * rhs.x
    };
}

Vec2
Vec2::normalize() const
{
    const f32 length_squared {get_length_squared()};

    if (length_squared < EPSILON_SQUARED)
    {
        return {0.0f, 0.0f};
    }

    const f32 length {sqrt(length_squared)};

    return {
        x / length,
        y / length,
    };
}

Vec3
Vec3::normalize() const
{
    const f32 length_squared {get_length_squared()};

    if (length_squared < EPSILON_SQUARED)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    const f32 length {sqrt(length_squared)};

    return {
        x / length,
        y / length,
        z / length
    };
}

Mat4
operator*(const Mat4& lhs, const Mat4& rhs)
{
    Mat4 result {};

    for (s32 column = 0; column < 4; ++column)
    {
        for (s32 row = 0; row < 4; ++row)
        {
            for (s32 i = 0; i < 4; ++i)
            {
                result[column][row] += lhs[i][row] * rhs[column][i];
            }
        }
    }

    return result;
}

Mat4 
Mat4::translate(const Vec3& s) const
{
    Mat4 translation_matrix {};

    translation_matrix[0][0] = 1.0f;
    translation_matrix[1][1] = 1.0f;
    translation_matrix[2][2] = 1.0f;
    translation_matrix[3][3] = 1.0f;
    translation_matrix[3][0] = s.x;
    translation_matrix[3][1] = s.y;
    translation_matrix[3][2] = s.z;

    return *this * translation_matrix;
}

Mat4
Mat4::rotate(const f32 angle, const Vec3& axis) const
{
    const Vec3 axis_normalized {axis.normalize()};

    const f32 cos_angle {cosf(angle)};
    const f32 sin_angle {sinf(angle)};
    const f32 tan_angle {1.0f - cos_angle};

    Mat4 rotation {};

    rotation[0][0] = cos_angle + axis_normalized.x * axis_normalized.x * tan_angle;
    rotation[0][1] = axis_normalized.x * axis_normalized.y * tan_angle + axis_normalized.z * sin_angle;
    rotation[0][2] = axis_normalized.x * axis_normalized.z * tan_angle - axis_normalized.y * sin_angle;

    rotation[1][0] = axis_normalized.y * axis_normalized.x * tan_angle - axis_normalized.z * sin_angle;
    rotation[1][1] = cos_angle + axis_normalized.y * axis_normalized.y * tan_angle;
    rotation[1][2] = axis_normalized.y * axis_normalized.z * tan_angle + axis_normalized.x * sin_angle;

    rotation[2][0] = axis_normalized.z * axis_normalized.x * tan_angle + axis_normalized.y * sin_angle;
    rotation[2][1] = axis_normalized.z * axis_normalized.y * tan_angle - axis_normalized.x * sin_angle;
    rotation[2][2] = cos_angle + axis_normalized.z * axis_normalized.z * tan_angle;

    rotation[3][3] = 1.0f;

    return *this * rotation;
}

Mat4
Mat4::make_diagonal(const f32 s)
{
    return {
        {
            {s, 0.0f, 0.0f, 0.0f },
            {0.0f, s, 0.0f, 0.0f },
            {0.0f, 0.0f, s, 0.0f },
            {0.0f, 0.0f, 0.0f, s },
        }
    };
}

Mat4 
get_view_matrix(const Vec3& position, const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 center = position + forward;

    return get_look_at_matrix(position, center, Vec3::unit_z());
}

Mat4 get_orthographic_matrix(const Vec2& min, const Vec2& max, const f32 near, const f32 far)
{
    Mat4 result {};
    result[0][0] = 2.0f / (max.x - min.x);
    result[1][1] = -2.0f / (max.y - min.y);
    result[2][2] = -2.0f / (far - near);
    result[3][0] = -(max.x + min.x) / (max.x - min.x);
    result[3][1] = (max.y + min.y) / (max.y - min.y);
    result[3][2] = -(far + near) / (far - near);
    result[3][3] = 1.0f;
    
    return result;
}

Mat4
get_projection_matrix(const f32 fov_y, const f32 aspect, const f32 near, const f32 far)
{
    const f32 tan_half_fov_y {tan(fov_y / 2.0f)};
    
    Mat4 result {};
    result[0][0] = 1.0f / (aspect * tan_half_fov_y);
    result[1][1] = 1.0f / (tan_half_fov_y);
    result[2][2] = -(far + near) / (far - near);
    result[2][3] = -1.0f;
    result[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

Mat4 
get_look_at_matrix(const Vec3& position, const Vec3& target, const Vec3& up)
{
    const Vec3 camera_forward = (target - position).normalize();
    const Vec3 camera_right = cross(camera_forward, up).normalize();
    const Vec3 camera_up = cross(camera_right, camera_forward);

    Mat4 result = {};

    result[0][0] = camera_right.x;
    result[1][0] = camera_right.y;
    result[2][0] = camera_right.z;

    result[0][1] = camera_up.x;
    result[1][1] = camera_up.y;
    result[2][1] = camera_up.z;

    result[0][2] = -camera_forward.x;
    result[1][2] = -camera_forward.y;
    result[2][2] = -camera_forward.z;

    result[3][0] = -dot(camera_right, position);
    result[3][1] = -dot(camera_up, position);
    result[3][2] = dot(camera_forward, position);

    result[3][3] = 1.0f;

    return result;
}

f32 
interpolate_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    const f32 alpha {1.0f - exp(-speed * delta_time)};

    if (
        f32 delta {target - current};
        abs(delta) > EPSILON
    ) {
        delta = fmod(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    
    return target;
}

Vec3 
get_forward(const Vec3& rotation)
{
    const f32 rotation_x {to_radians(rotation.x)};
    const f32 rotation_z {to_radians(rotation.z)};

    const Vec3 forward {
        cos(rotation_x) * cos(rotation_z),
        cos(rotation_x) * sin(rotation_z),
        sin(rotation_x),
    };

    return forward.normalize();
}

Vec3 
get_right(const Vec3& rotation)
{
    const Vec3 forward {get_forward(rotation)};
    const Vec3 right {cross(forward, Vec3::unit_z())};

    return right.normalize();
}

Vec3 
get_up(const Vec3& rotation)
{
    const Vec3 forward {get_forward(rotation)};
    const Vec3 right {get_right(rotation)};
    const Vec3 up {cross(forward, right)};

    return up.normalize();
}

IVec2 IBounds2::position() const
{
    return {
        min.x,
        min.y
    };
}

IVec2
IBounds2::size() const
{
    return {
        max.x - min.x,
        max.y - min.y,
    };
}

b32
overlaps(const IBounds2& lhs, const IBounds2& rhs)
{
    const b32 is_clear {
        lhs.max.x <= rhs.min.x ||
        rhs.max.x <= lhs.min.x ||
        lhs.max.y <= rhs.min.y ||
        rhs.max.y <= lhs.min.y
    };

    return !is_clear;
}

IBounds2
intersection(const IBounds2& lhs, const IBounds2& rhs)
{
    const IVec2 o_min {
        max(lhs.min.x, rhs.min.x),
        max(lhs.min.y, rhs.min.y)
    };

    const IVec2 o_max {
        min(lhs.max.x, rhs.max.x),
        min(lhs.max.y, rhs.max.y)
    };

    return {
        {o_min.x, o_min.y},
        {o_max.x, o_max.y},
    };
}

vector<IBounds2>
subtract(const IBounds2& lhs, const IBounds2& rhs)
{
    if (!overlaps(lhs, rhs))
    {
        return {lhs};
    }

    vector<IBounds2> bounds_vector {};
    const IBounds2 intersection_bounds {intersection(lhs, rhs)};

    if (intersection_bounds.min.x > lhs.min.x)
    {
        bounds_vector.push_back({
            {lhs.min.x, lhs.min.y},
            {intersection_bounds.min.x, lhs.max.y},
        });
    }

    if (intersection_bounds.max.x < lhs.max.x)
    {
        bounds_vector.push_back({
            {intersection_bounds.max.x, lhs.min.y},
            {lhs.max.x, lhs.max.y}
        });
    }

    if (intersection_bounds.min.y > lhs.min.y)
    {
        bounds_vector.push_back({
            {intersection_bounds.min.x, lhs.min.y},
            {intersection_bounds.max.x, intersection_bounds.min.y}
        });
    }

    if (intersection_bounds.max.y < lhs.max.y)
    {
        bounds_vector.push_back({
            {intersection_bounds.min.x, intersection_bounds.max.y},
            {intersection_bounds.max.x, lhs.max.y}
        });
    }

    return bounds_vector;
}
