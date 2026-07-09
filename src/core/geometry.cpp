#include "geometry.h"

#include <cmath>

using namespace std;

Vec2::Vec2()
    :
    m_x{0.0f},
    m_y{0.0f}
{

}

Vec2::Vec2(const f32 s)
    :
    m_x{s},
    m_y{s}
{

}

Vec2::Vec2(f32 x, f32 y)
    :
    m_x{x},
    m_y{y}
{

}

Vec2::Vec2(const IVec2& ivec2)
    :
    m_x{static_cast<f32>(ivec2.m_x)},
    m_y{static_cast<f32>(ivec2.m_y)}
{

}

Vec3::Vec3()
    :
    m_x{0.0f},
    m_y{0.0f},
    m_z{0.0f}
{

}

Vec3::Vec3(const f32 s)
    :
    m_x{s},
    m_y{s},
    m_z{s}
{

}

Vec3::Vec3(const f32 x, const f32 y, const f32 z)
    :
    m_x(x),
    m_y(y),
    m_z(z)
{

}

Vec3::Vec3(const IVec3& ivec3)
    :
    m_x{static_cast<f32>(ivec3.m_x)},
    m_y{static_cast<f32>(ivec3.m_y)},
    m_z{static_cast<f32>(ivec3.m_z)}
{

}

IVec3::IVec3()
    :
    m_x{0},
    m_y{0},
    m_z{0}
{

}

IVec3::IVec3(const s32 s)
    :
    m_x{s},
    m_y{s},
    m_z{s}
{

}

IVec3::IVec3(const s32 x, const s32 y, const s32 z)
    :
    m_x{x},
    m_y{y},
    m_z{z}
{

}

Vec2
operator+(const Vec2& lhs, const Vec2& rhs)
{
    return {
        lhs.m_x + rhs.m_x,
        lhs.m_y + rhs.m_y,
    };
}

Vec3 
operator+(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.m_x + rhs.m_x,
        lhs.m_y + rhs.m_y,
        lhs.m_z + rhs.m_z
    };
}

IVec2 operator+(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.m_x + rhs.m_x,
        lhs.m_y + rhs.m_y,
    };
}

IVec3 operator+(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.m_x + rhs.m_x,
        lhs.m_y + rhs.m_y,
        lhs.m_z + rhs.m_z
    };
}

Vec2 
operator-(const Vec2& lhs, const Vec2& rhs)
{
    return {
        lhs.m_x - rhs.m_x,
        lhs.m_y - rhs.m_y,
    };
}

Vec3 
operator-(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.m_x - rhs.m_x,
        lhs.m_y - rhs.m_y,
        lhs.m_z - rhs.m_z
    };
}

IVec2 operator-(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.m_x - rhs.m_x,
        lhs.m_y - rhs.m_y,
    };
}

IVec3 operator-(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.m_x - rhs.m_x,
        lhs.m_y - rhs.m_y,
        lhs.m_z - rhs.m_z
    };
}

Vec2
operator*(const f32 lhs, const Vec2& rhs)
{
    return {
        lhs * rhs.m_x,
        lhs * rhs.m_y,
    };
}

Vec3
operator*(const f32 lhs, const Vec3& rhs)
{
    return {
        lhs * rhs.m_x,
        lhs * rhs.m_y,
        lhs * rhs.m_z,
    };
}

IVec2 operator*(const s32 lhs, const IVec2& rhs)
{
    return {
        lhs * rhs.m_x,
        lhs * rhs.m_y,
    };
}

IVec3 operator*(const s32 lhs, const IVec3& rhs)
{
    return {
        lhs * rhs.m_x,
        lhs * rhs.m_y,
        lhs * rhs.m_z,
    };
}

f32 
Vec2::get_length() const
{
    return sqrt(m_x * m_x + m_y * m_y);
}

f32 
Vec3::get_length() const
{
    return sqrt(m_x * m_x + m_y * m_y + m_z * m_z);
}

f32
Vec2::get_length_squared() const
{
    return m_x * m_x + m_y * m_y;
}

f32
Vec3::get_length_squared() const
{
    return m_x * m_x + m_y * m_y + m_z * m_z;
}

Vec3
max(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.m_x > rhs.m_x ? lhs.m_x : rhs.m_x,
        lhs.m_y > rhs.m_y ? lhs.m_y : rhs.m_y,
        lhs.m_z > rhs.m_z ? lhs.m_z : rhs.m_z,
    };
}

Vec3
min(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.m_x < rhs.m_x ? lhs.m_x : rhs.m_x,
        lhs.m_y < rhs.m_y ? lhs.m_y : rhs.m_y,
        lhs.m_z < rhs.m_z ? lhs.m_z : rhs.m_z,
    };
}

f32
dot(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.m_x * rhs.m_x + lhs.m_y * rhs.m_y;
}

f32 
dot(const Vec3& lhs, const Vec3& rhs)
{
    return lhs.m_x * rhs.m_x + lhs.m_y * rhs.m_y + lhs.m_z * rhs.m_z;
}

Vec3 
cross(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.m_y * rhs.m_z - lhs.m_z * rhs.m_y,
        lhs.m_z * rhs.m_x - lhs.m_x * rhs.m_z,
        lhs.m_x * rhs.m_y - lhs.m_y * rhs.m_x
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
        m_x / length,
        m_y / length,
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
        m_x / length,
        m_y / length,
        m_z / length
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
    translation_matrix[3][0] = s.m_x;
    translation_matrix[3][1] = s.m_y;
    translation_matrix[3][2] = s.m_z;

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

    rotation[0][0] = cos_angle + axis_normalized.m_x * axis_normalized.m_x * tan_angle;
    rotation[0][1] = axis_normalized.m_x * axis_normalized.m_y * tan_angle + axis_normalized.m_z * sin_angle;
    rotation[0][2] = axis_normalized.m_x * axis_normalized.m_z * tan_angle - axis_normalized.m_y * sin_angle;

    rotation[1][0] = axis_normalized.m_y * axis_normalized.m_x * tan_angle - axis_normalized.m_z * sin_angle;
    rotation[1][1] = cos_angle + axis_normalized.m_y * axis_normalized.m_y * tan_angle;
    rotation[1][2] = axis_normalized.m_y * axis_normalized.m_z * tan_angle + axis_normalized.m_x * sin_angle;

    rotation[2][0] = axis_normalized.m_z * axis_normalized.m_x * tan_angle + axis_normalized.m_y * sin_angle;
    rotation[2][1] = axis_normalized.m_z * axis_normalized.m_y * tan_angle - axis_normalized.m_x * sin_angle;
    rotation[2][2] = cos_angle + axis_normalized.m_z * axis_normalized.m_z * tan_angle;

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
    result[0][0] = 2.0f / (max.m_x - min.x());
    result[1][1] = -2.0f / (max.m_y - min.m_y);
    result[2][2] = -2.0f / (far - near);
    result[3][0] = -(max.m_x + min.x()) / (max.m_x - min.x());
    result[3][1] = (max.m_y + min.m_y) / (max.m_y - min.m_y);
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

    result[0][0] = camera_right.m_x;
    result[1][0] = camera_right.m_y;
    result[2][0] = camera_right.m_z;

    result[0][1] = camera_up.m_x;
    result[1][1] = camera_up.m_y;
    result[2][1] = camera_up.m_z;

    result[0][2] = -camera_forward.m_x;
    result[1][2] = -camera_forward.m_y;
    result[2][2] = -camera_forward.m_z;

    result[3][0] = -dot(camera_right, position);
    result[3][1] = -dot(camera_up, position);
    result[3][2] = dot(camera_forward, position);

    result[3][3] = 1.0f;

    return result;
}

f32 
interpolate_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > EPSILON)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    
    return target;
}

Vec3 
get_forward(const Vec3& rotation)
{
    const f32 rotation_x = to_radians(rotation.x());
    const f32 rotation_z = to_radians(rotation.z());

    const Vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return forward.normalize();
}

Vec3 
get_right(const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 right = cross(forward, Vec3::unit_z());

    return right.normalize();
}

Vec3 
get_up(const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 right = get_right(rotation);
    const Vec3 up = cross(forward, right);

    return up.normalize();
}

IVec2 IBounds2::position() const
{
    return {
        m_min.m_x,
        m_min.m_y
    };
}

IVec2
IBounds2::size() const
{
    return {
        m_max.m_x - m_min.m_x,
        m_max.m_y - m_min.m_y,
    };
}

b32
overlaps(const IBounds2& lhs, const IBounds2& rhs)
{
    return !(
        lhs.m_max.m_x <= rhs.m_min.m_x ||
        rhs.m_max.m_x <= lhs.m_min.m_x ||
        lhs.m_max.m_y <= rhs.m_min.m_y ||
        rhs.m_max.m_y <= lhs.m_min.m_y
    );
}

IBounds2
intersection(const IBounds2& lhs, const IBounds2& rhs)
{
    const IVec2 o_min = {
        max(lhs.m_min.m_x, rhs.m_min.m_x),
        max(lhs.m_min.m_y, rhs.m_min.m_y)
    };

    const IVec2 o_max = {
        min(lhs.m_max.m_x, rhs.m_max.m_x),
        min(lhs.m_max.m_y, rhs.m_max.m_y)
    };

    return {
        {o_min.m_x, o_min.m_y},
        {o_max.m_x, o_max.m_y},
    };
}

vector<IBounds2>
subtract(const IBounds2& lhs, const IBounds2& rhs)
{
    vector<IBounds2> bounds_vector {};

    if (!overlaps(lhs, rhs))
    {
        return bounds_vector;
    }

    const IBounds2 intersection_bounds {intersection(lhs, rhs)};

    if (intersection_bounds.m_min.m_x > lhs.m_min.m_x)
    {
        bounds_vector.push_back({
            {lhs.m_min.m_x, lhs.m_min.m_y},
            {intersection_bounds.m_min.m_x, lhs.m_min.m_y},
        });
    }

    if (intersection_bounds.m_max.m_x < lhs.m_max.m_x)
    {
        bounds_vector.push_back({
            {intersection_bounds.m_max.m_x, lhs.m_max.m_y},
            {lhs.m_max.m_x, lhs.m_max.m_y}
        });
    }

    if (intersection_bounds.m_min.m_y > lhs.m_min.m_y)
    {
        bounds_vector.push_back({
            {intersection_bounds.m_min.m_x, lhs.m_min.m_y},
            {intersection_bounds.m_max.m_x, intersection_bounds.m_min.m_y}
        });
    }

    if (intersection_bounds.m_max.m_y < lhs.m_max.m_y)
    {
        bounds_vector.push_back({
            {intersection_bounds.m_min.m_x, intersection_bounds.m_max.m_y},
            {intersection_bounds.m_max.m_x, lhs.m_max.m_y}
        });
    }

    return bounds_vector;
}
