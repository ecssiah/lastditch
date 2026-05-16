#include "geometry.h"

#include <cmath>

Vec2
operator+(const Vec2& a, const Vec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

Vec3 
operator+(const Vec3& a, const Vec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

IVec2 operator+(const IVec2& a, const IVec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

IVec3 operator+(const IVec3& a, const IVec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

Vec2 
operator-(const Vec2& a, const Vec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

Vec3 
operator-(const Vec3& a, const Vec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

IVec2 operator-(const IVec2& a, const IVec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

IVec3 operator-(const IVec3& a, const IVec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

Vec2
operator*(const f32 scalar, const Vec2& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
    };
}

Vec3
operator*(const f32 scalar, const Vec3& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
        scalar * a.z,
    };
}

IVec2 operator*(const i32 scalar, const IVec2& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
    };
}

IVec3 operator*(const i32 scalar, const IVec3& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
        a.z * scalar
    };
}

Vec2 ivec2_to_vec2(const IVec2& a)
{
    return {
        static_cast<f32>(a.x),
        static_cast<f32>(a.y)
    };
}

Vec3 ivec3_to_vec3(const IVec3& a)
{
    return {
        static_cast<f32>(a.x),
        static_cast<f32>(a.y),
        static_cast<f32>(a.z)
    };
}

Vec3 vec3_max(const Vec3& a, const Vec3& b)
{
    return {
        a.x > b.x ? a.x : b.x,
        a.y > b.y ? a.y : b.y,
        a.z > b.z ? a.z : b.z,
    };
}

Vec3 vec3_min(const Vec3& a, const Vec3& b)
{
    return {
        a.x < b.x ? a.x : b.x,
        a.y < b.y ? a.y : b.y,
        a.z < b.z ? a.z : b.z,
    };
}

f32 
vec2_length(const Vec2& a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

f32 
vec3_length(const Vec3& a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32
vec2_length_squared(const Vec2& a)
{
    return a.x * a.x + a.y * a.y;
}

f32
vec3_length_squared(const Vec3& a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

Vec2
vec2_normalize(const Vec2& a)
{
    const f32 a_length_squared = vec2_length_squared(a);
    
    if (a_length_squared < epsilon_squared)
    {
        return {0.0f, 0.0f};
    }
    
    const f32 a_length = sqrt(a_length_squared);
    
    return {
        a.x / a_length,
        a.y / a_length,
    };
}

Vec3 
vec3_normalize(const Vec3& a)
{
    const f32 a_length_squared = vec3_length_squared(a);
    
    if (a_length_squared < epsilon_squared)
    {
        return { 0.0f, 0.0f, 0.0f };
    }
    
    const f32 a_length = sqrt(a_length_squared);
    
    return {
        a.x / a_length,
        a.y / a_length,
        a.z / a_length
    };
}

f32 
vec2_dot(const Vec2& a, const Vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

f32 
vec3_dot(const Vec3& a, const Vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

Vec3 
vec3_cross(const Vec3& a, const Vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

Mat4

operator*(const Mat4& a, const Mat4& b)
{
    Mat4 result = {};

    for (i32 column = 0; column < 4; ++column)
    {
        for (i32 row = 0; row < 4; ++row)
        {
            for (i32 i = 0; i < 4; ++i)
            {
                result[column][row] += a[i][row] * b[column][i];
            }
        }
    }

    return result;
}

Mat4 
mat4_translate(const Mat4& a, const Vec3& translation)
{
    Mat4 translation_matrix = {};

    translation_matrix[0][0] = 1.0f;
    translation_matrix[1][1] = 1.0f;
    translation_matrix[2][2] = 1.0f;
    translation_matrix[3][3] = 1.0f;
    translation_matrix[3][0] = translation.x;
    translation_matrix[3][1] = translation.y;
    translation_matrix[3][2] = translation.z;

    return a * translation_matrix;
}

Mat4
mat4_rotate(const Mat4& a, const f32 angle, const Vec3& axis)
{
    const Vec3 axis_normalized = vec3_normalize(axis);

    const f32 cos_angle = cosf(angle);
    const f32 sin_angle = sinf(angle);
    const f32 tan_angle = 1.0f - cos_angle;

    Mat4 rotation = {};

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

    return a * rotation;
}

Mat4 
get_view_matrix(const Vec3& position, const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 center = position + forward;

    return get_look_at_matrix(position, center, unit_z);
}

Mat4 get_orthographic_matrix(const Vec2& min, const Vec2& max, const f32 near, const f32 far)
{
    Mat4 result = {};
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
    const f32 tan_half_fov_y = tan(fov_y / 2.0f);
    
    Mat4 result = {};
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
    const Vec3 camera_forward = vec3_normalize(target - position);
    const Vec3 camera_right = vec3_normalize(vec3_cross(camera_forward, up));
    const Vec3 camera_up = vec3_cross(camera_right, camera_forward);

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

    result[3][0] = -vec3_dot(camera_right, position);
    result[3][1] = -vec3_dot(camera_up, position);
    result[3][2] = vec3_dot(camera_forward, position);

    result[3][3] = 1.0f;

    return result;
}

f32 
interpolate_to(const f32 current, const f32 target, const f32 speed, const f32 delta_time)
{
    const f32 alpha = 1.0f - expf(-speed * delta_time);
    f32 delta = target - current;

    if (fabsf(delta) > epsilon)
    {
        delta = fmodf(delta + 180.0f, 360.0f) - 180.0f;

        return current + delta * alpha;
    }
    
    return target;
}

Vec3 
get_forward(const Vec3& rotation)
{
    const f32 rotation_x = to_radians(rotation.x);
    const f32 rotation_z = to_radians(rotation.z);

    const Vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return vec3_normalize(forward);
}

Vec3 
get_right(const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 right = vec3_cross(forward, unit_z);

    return vec3_normalize(right);
}

Vec3 
get_up(const Vec3& rotation)
{
    const Vec3 forward = get_forward(rotation);
    const Vec3 right = get_right(rotation);
    const Vec3 up = vec3_cross(forward, right);

    return vec3_normalize(up);
}

IVec2 
ibounds2_size(const IBounds2& a)
{
    return {
        a.max.x - a.min.x,
        a.max.y - a.min.y,
    };
}

b32 
ibounds2_overlaps(const IBounds2& a, const IBounds2& b)
{
    return !(
        a.max.x <= b.min.x ||
        b.max.x <= a.min.x ||
        a.max.y <= b.min.y ||
        b.max.y <= a.min.y
    );
}

IBounds2 
ibounds2_intersection(const IBounds2& a, const IBounds2& b)
{
    const IVec2 o_min = {
        max_i32(a.min.x, b.min.x),
        max_i32(a.min.y, b.min.y)
    };

    const IVec2 o_max = {
        min_i32(a.max.x, b.max.x),
        min_i32(a.max.y, b.max.y)
    };

    return {
        {o_min.x, o_min.y},
        {o_max.x, o_max.y},
    };
}

std::vector<IBounds2> 
ibounds2_subtract(const IBounds2& a, const IBounds2& b)
{
    std::vector<IBounds2> bounds_vector;

    if (!ibounds2_overlaps(a, b))
    {
        return bounds_vector;
    }

    const IBounds2 intersection = ibounds2_intersection(a, b);

    if (intersection.min.x > a.min.x)
    {
        bounds_vector.push_back({
            .min = {a.min.x, a.min.y},
            .max = {intersection.min.x, a.max.y},
        });
    }

    if (intersection.max.x < a.max.x)
    {
        bounds_vector.push_back({
            .min = {intersection.max.x, a.min.y},
            .max = {a.max.x, a.max.y}
        });
    }

    if (intersection.min.y > a.min.y)
    {
        bounds_vector.push_back({
            .min = {intersection.min.x, a.min.y},
            .max = {intersection.max.x, intersection.min.y}
        });
    }

    if (intersection.max.y < a.max.y)
    {
        bounds_vector.push_back({
            .min = {intersection.min.x, intersection.max.y},
            .max = {intersection.max.x, a.max.y}
        });
    }

    return bounds_vector;
}
