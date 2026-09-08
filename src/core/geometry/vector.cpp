#include "vector.h"

#include <cmath>

using namespace std;

Vec2::Vec2(const IVec2& vec)
    :
    x { static_cast<f32>(vec.x) },
    y { static_cast<f32>(vec.y) }
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

Vec2
operator-(const Vec2& lhs, const Vec2& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
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

f32
Vec2::get_length() const
{
    return sqrt(x * x + y * y);
}

f32
Vec2::get_length_squared() const
{
    return x * x + y * y;
}

f32
dot(const Vec2& lhs, const Vec2& rhs)
{
    return lhs.x * rhs.x + lhs.y * rhs.y;
}

Vec2
Vec2::normalize() const
{
    constexpr f32 epsilon { std::numeric_limits<f32>::epsilon() };

    const f32 length_squared { get_length_squared() };

    if (length_squared < epsilon)
    {
        return { 0.0f, 0.0f };
    }

    const f32 length { sqrt(length_squared) };

    return {
        x / length,
        y / length,
    };
}

Vec3::Vec3(const IVec3& vec)
    :
    x { static_cast<f32>(vec.x) },
    y { static_cast<f32>(vec.y) },
    z { static_cast<f32>(vec.z) }
{

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

Vec3
operator-(const Vec3& lhs, const Vec3& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
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

f32
Vec3::get_length() const
{
    return sqrt(x * x + y * y + z * z);
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

Vec3
Vec3::normalize() const
{
    constexpr f32 epsilon { std::numeric_limits<f32>::epsilon() };

    const f32 length_squared { get_length_squared() };

    if (length_squared < epsilon)
    {
        return { 0.0f, 0.0f, 0.0f };
    }

    const f32 length { sqrt(length_squared) };

    return {
        x / length,
        y / length,
        z / length
    };
}

IVec2
operator+(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
    };
}

IVec2
operator-(const IVec2& lhs, const IVec2& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
    };
}

IVec2
operator*(const s32 lhs, const IVec2& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
    };
}

IVec3
operator+(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.x + rhs.x,
        lhs.y + rhs.y,
        lhs.z + rhs.z
    };
}

IVec3
operator-(const IVec3& lhs, const IVec3& rhs)
{
    return {
        lhs.x - rhs.x,
        lhs.y - rhs.y,
        lhs.z - rhs.z
    };
}

IVec3
operator*(const s32 lhs, const IVec3& rhs)
{
    return {
        lhs * rhs.x,
        lhs * rhs.y,
        lhs * rhs.z,
    };
}
