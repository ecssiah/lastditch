#include "geometry.h"

#include <cmath>

vec2
operator+(const vec2& a, const vec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

vec3 
operator+(const vec3& a, const vec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

ivec2 operator+(const ivec2& a, const ivec2& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
    };
}

ivec3 operator+(const ivec3& a, const ivec3& b)
{
    return {
        a.x + b.x,
        a.y + b.y,
        a.z + b.z
    };
}

vec2 
operator-(const vec2& a, const vec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

vec3 
operator-(const vec3& a, const vec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

ivec2 operator-(const ivec2& a, const ivec2& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
    };
}

ivec3 operator-(const ivec3& a, const ivec3& b)
{
    return {
        a.x - b.x,
        a.y - b.y,
        a.z - b.z
    };
}

vec2
operator*(const f32 scalar, const vec2& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
    };
}

vec3
operator*(const f32 scalar, const vec3& a)
{
    return {
        scalar * a.x,
        scalar * a.y,
        scalar * a.z,
    };
}

ivec2 operator*(i32 scalar, const ivec2& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
    };
}

ivec3 operator*(i32 scalar, const ivec3& a)
{
    return {
        a.x * scalar,
        a.y * scalar,
        a.z * scalar
    };
}



f32 
length(const vec2& a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

f32 
length(const vec3& a)
{
    return sqrt(a.x * a.x + a.y * a.y + a.z * a.z);
}

f32
length_squared(const vec2& a)
{
    return a.x * a.x + a.y * a.y;
}

f32
length_squared(const vec3& a)
{
    return a.x * a.x + a.y * a.y + a.z * a.z;
}

vec2
normalize(const vec2& a)
{
    const f32 a_length_squared = length_squared(a);
    
    if (a_length_squared < EPSILON)
    {
        return {0.0f, 0.0f};
    }
    
    const f32 a_length = sqrt(a_length_squared);
    
    return {
        a.x / a_length,
        a.y / a_length,
    };
}

vec3 
normalize(const vec3& a)
{
    const f32 a_length_squared = length_squared(a);
    
    if (a_length_squared < EPSILON)
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
dot(const vec2& a, const vec2& b)
{
    return a.x * b.x + a.y * b.y;
}

f32 
dot(const vec3& a, const vec3& b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

vec3 
cross(const vec3& a, const vec3& b)
{
    return {
        a.y * b.z - a.z * b.y,
        a.z * b.x - a.x * b.z,
        a.x * b.y - a.y * b.x
    };
}

mat4

operator*(const mat4& a, const mat4& b)
{
    mat4 result = {};

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

mat4 
mat4_translate(const mat4& a, const vec3& translation)
{
    mat4 translation_matrix = {};

    translation_matrix[0][0] = 1.0f;
    translation_matrix[1][1] = 1.0f;
    translation_matrix[2][2] = 1.0f;
    translation_matrix[3][3] = 1.0f;
    translation_matrix[3][0] = translation.x;
    translation_matrix[3][1] = translation.y;
    translation_matrix[3][2] = translation.z;

    return a * translation_matrix;
}

mat4
mat4_rotate(const mat4& a, const vec3& axis, f32 angle)
{
    const vec3 n = normalize(axis);

    const f32 c = cosf(angle);
    const f32 s = sinf(angle);
    const f32 t = 1.0f - c;

    mat4 rotation = {};

    rotation[0][0] = c + n.x * n.x * t;
    rotation[0][1] = n.x * n.y * t + n.z * s;
    rotation[0][2] = n.x * n.z * t - n.y * s;

    rotation[1][0] = n.y * n.x * t - n.z * s;
    rotation[1][1] = c + n.y * n.y * t;
    rotation[1][2] = n.y * n.z * t + n.x * s;

    rotation[2][0] = n.z * n.x * t + n.y * s;
    rotation[2][1] = n.z * n.y * t - n.x * s;
    rotation[2][2] = c + n.z * n.z * t;

    rotation[3][3] = 1.0f;

    return a * rotation;
}

mat4 
orthographic_matrix(const f32 left, const f32 right, const f32 bottom, const f32 top, const f32 near, const f32 far)
{
    mat4 result = {};
    result[0][0] = 2.0f / (right - left);
    result[1][1] = 2.0f / (top - bottom);
    result[2][2] = -2.0f / (far - near);
    result[3][0] = -(right + left) / (right - left);
    result[3][1] = -(top + bottom) / (top - bottom);
    result[3][2] = -(far + near) / (far - near);
    
    return result;
}

mat4
projection_matrix(const f32 fovy, const f32 aspect, const f32 near, const f32 far)
{
    const f32 tan_half_fovy = tan(fovy / 2.0f);
    
    mat4 result = {};
    result[0][0] = 1.0f / (aspect * tan_half_fovy);
    result[1][1] = 1.0f / (tan_half_fovy);
    result[2][2] = -(far + near) / (far - near);
    result[2][3] = -1.0f;
    result[3][2] = -(2.0f * far * near) / (far - near);

    return result;
}

mat4 
look_at(const vec3 position, const vec3 target, const vec3 up)
{
    const vec3 forward = normalize(target - position);
    const vec3 right = normalize(cross(forward, up));
    const vec3 camera_up = cross(right, forward);

    mat4 result = {};

    result[0][0] = right.x;
    result[1][0] = right.y;
    result[2][0] = right.z;

    result[0][1] = camera_up.x;
    result[1][1] = camera_up.y;
    result[2][1] = camera_up.z;

    result[0][2] = -forward.x;
    result[1][2] = -forward.y;
    result[2][2] = -forward.z;

    result.elements[3][0] = -dot(right, position);
    result.elements[3][1] = -dot(camera_up, position);
    result.elements[3][2] = dot(forward, position);

    result.elements[3][3] = 1.0f;

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

vec3 
get_forward(const vec3& rotation)
{
    const f32 rotation_x = to_radians(rotation.x);
    const f32 rotation_z = to_radians(rotation.z);

    const vec3 forward = {
        cosf(rotation_x) * cosf(rotation_z),
        cosf(rotation_x) * sinf(rotation_z),
        sinf(rotation_x),
    };

    return normalize(forward);
}

vec3 
get_right(const vec3& rotation)
{
    const vec3 forward = get_forward(rotation);
    const vec3 right = cross(forward, UNIT_Z);

    return normalize(right);
}

vec3 
get_up(const vec3& rotation)
{
    const vec3 forward = get_forward(rotation);
    const vec3 right = get_right(rotation);
    const vec3 up = cross(forward, right);

    return normalize(up);
}


ivec2
irange2_max(const irange2& a)
{
    return {
        a.position.x + a.size.x,
        a.position.y + a.size.y,
    };
}

ivec2 
irange2_min(const irange2& a)
{
    return {
        a.position.x,
        a.position.y,
    };
}

b32 
irange2_overlaps(const irange2& a, const irange2& b)
{
    const ivec2 left_min = irange2_min(a);
    const ivec2 left_max = irange2_max(a);

    const ivec2 right_min = irange2_min(b);
    const ivec2 right_max = irange2_max(b);

    return !(
        left_max.x <= right_min.x ||
        right_max.x <= left_min.x ||
        left_max.y <= right_min.y ||
        right_max.y <= left_min.y
    );
}

irange2 
irange2_intersection(const irange2& a, const irange2& b)
{
    const ivec2 left_min = irange2_min(a);
    const ivec2 left_max = irange2_max(a);

    const ivec2 right_min = irange2_min(b);
    const ivec2 right_max = irange2_max(b);

    const ivec2 o_min = {
        max_i32(left_min.x, right_min.x),
        max_i32(left_min.y, right_min.y)
    };

    const ivec2 o_max = {
        min_i32(left_max.x, right_max.x),
        min_i32(left_max.y, right_max.y)
    };

    return {
        {o_min.x, o_min.y},
        {o_max.x - o_min.x, o_max.y - o_min.y},
    };
}

std::vector<irange2> 
irange2_subtract(const irange2& a, const irange2& b)
{
    std::vector<irange2> range_vector = std::vector<irange2>();

    const ivec2 left_min = irange2_min(a);
    const ivec2 left_max = irange2_max(a);

    if (!irange2_overlaps(a, b))
    {
        return range_vector;
    }

    const irange2 intersection = irange2_intersection(a, b);

    const ivec2 intersection_min = irange2_min(intersection);
    const ivec2 intersection_max = irange2_max(intersection);

    if (intersection_min.x > left_min.x)
    {
        range_vector.push_back({
            .position = {left_min.x, left_min.y},
            .size = {intersection_min.x - left_min.x, left_max.y - left_min.y}
        });
    }

    if (intersection_max.x < left_max.x)
    {
        range_vector.push_back({
            .position = {intersection_max.x, left_min.y},
            .size = {left_max.x - intersection_max.x, left_max.y - left_min.y}
        });
    }

    if (intersection_min.y > left_min.y)
    {
        range_vector.push_back({
            .position = {intersection_min.x, left_min.y},
            .size = {intersection_max.x - intersection_min.x, intersection_min.y - left_min.y}
        });
    }

    if (intersection_max.y < left_max.y)
    {
        range_vector.push_back({
            .position = {intersection_min.x, intersection_max.y},
            .size = {intersection_max.x - intersection_min.x, left_max.y - intersection_max.y}
        });
    }

    return range_vector;
}
