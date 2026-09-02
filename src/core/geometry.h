#pragma once

#include <numbers>
#include <vector>

#include "types.h"

class IVec2;
class IVec3;
class Vec2;
class Vec3;

enum class Direction : u8
{
    East,
    West,
    North,
    South,
    Up,
    Down,
    COUNT,
};

constexpr s32 DIRECTION_COUNT { static_cast<s32>(Direction::COUNT) };

constexpr std::array DIRECTION_NORMAL_ARRAY
{
    +1.0f, +0.0f, +0.0f,
    -1.0f, +0.0f, +0.0f,
    +0.0f, +1.0f, +0.0f,
    +0.0f, -1.0f, +0.0f,
    +0.0f, +0.0f, +1.0f,
    +0.0f, +0.0f, -1.0f,
};

enum class Axis : u8
{
    X,
    Y,
    Z,
    COUNT,
};

constexpr s32 AXIS_COUNT { static_cast<s32>(Axis::COUNT) };

class Vec2
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };

    constexpr Vec2() = default;

    explicit constexpr Vec2(const f32 s)
        :
        x { s },
        y { s }
    {

    }

    constexpr Vec2(const f32 x, const f32 y)
        :
        x { x },
        y { y }
    {

    }

    explicit Vec2(const IVec2& vec);

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

    static constexpr Vec2
    unit_x()
    {
        return { 1.0f, 0.0f };
    }

    static constexpr Vec2
    unit_y()
    {
        return { 0.0f, 1.0f };
    }

    f32 get_length() const;
    f32 get_length_squared() const;
    Vec2 normalize() const;

    friend Vec2 operator+(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator-(const Vec2& lhs, const Vec2& rhs);
    friend Vec2 operator*(f32 scalar, const Vec2& a);

    friend f32 dot(const Vec2& lhs, const Vec2& rhs);
};

class Vec3
{
public:
    f32 x { 0.0f };
    f32 y { 0.0f };
    f32 z { 0.0f };

    constexpr Vec3() = default;

    explicit constexpr Vec3(const f32 s)
        :
        x { s },
        y { s },
        z { s }
    {

    }

    constexpr Vec3(const f32 x, const f32 y, const f32 z)
        :
        x { x },
        y { y },
        z { z }
    {

    }

    explicit constexpr Vec3(const f32 vec[4])
        :
        x { vec[0] },
        y { vec[1] },
        z { vec[2] }
    {

    }

    explicit Vec3(const IVec3& vec);

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

    static constexpr Vec3
    unit_x()
    {
        return { 1.0f, 0.0f, 0.0f };
    }

    static constexpr Vec3
    unit_y()
    {
        return { 0.0f, 1.0f, 0.0f };
    }

    static constexpr Vec3
    unit_z()
    {
        return { 0.0f, 0.0f, 1.0f };
    }

    f32 get_length() const;
    f32 get_length_squared() const;
    Vec3 normalize() const;

    friend Vec3 max(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 min(const Vec3& lhs, const Vec3& rhs);

    friend f32 dot(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 cross(const Vec3& lhs, const Vec3& rhs);

    friend Vec3 operator+(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 operator-(const Vec3& lhs, const Vec3& rhs);
    friend Vec3 operator*(f32 lhs, const Vec3& rhs);
};

class IVec2
{
public:
    s32 x { 0 };
    s32 y { 0 };

    constexpr IVec2() = default;

    explicit constexpr IVec2(const s32 s)
        :
        x { s },
        y { s }
    {

    }

    constexpr IVec2(const s32 x, const s32 y)
        :
        x { x },
        y { y }
    {

    }

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

    friend IVec2 operator+(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator-(const IVec2& lhs, const IVec2& rhs);
    friend IVec2 operator*(s32 lhs, const IVec2& rhs);
};

class IVec3
{
public:
    s32 x { 0 };
    s32 y { 0 };
    s32 z { 0 };

    constexpr IVec3() = default;

    explicit constexpr IVec3(const s32 s)
        :
        x { s },
        y { s },
        z { s }
    {

    }

    constexpr IVec3(const s32 x, const s32 y, const s32 z)
        :
        x { x },
        y { y },
        z { z }
    {

    }

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

    friend IVec3 operator+(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator-(const IVec3& lhs, const IVec3& rhs);
    friend IVec3 operator*(s32 lhs, const IVec3& rhs);
};

class Mat4
{
public:
    constexpr Mat4() = default;

    explicit constexpr Mat4(const f32 s)
    {
        entry_array[0][0] = s;
        entry_array[1][1] = s;
        entry_array[2][2] = s;
        entry_array[3][3] = s;
    }

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

    friend Mat4 operator*(const Mat4& lhs, const Mat4& rhs);

    Mat4 translate(const Vec3& translation) const;
    Mat4 rotate(f32 angle, const Vec3& axis) const;

private:
    f32 entry_array[4][4] {};
};

class IBounds2
{
public:
    IBounds2() = default;
    IBounds2(const IVec2& min, const IVec2& max);

    IVec2 position() const;
    IVec2 size() const;

    friend b32 overlaps(const IBounds2& lhs, const IBounds2& rhs);
    friend IBounds2 get_intersection(const IBounds2& lhs, const IBounds2& rhs);
    friend std::vector<IBounds2> subtract(const IBounds2& lhs, const IBounds2& rhs);

    IVec2 min {};
    IVec2 max {};
};

class IBounds3
{
public:
    IBounds3() = default;
    IBounds3(const IVec3& min, const IVec3& max);

    IVec3 position() const;
    IVec3 size() const;

    IVec3 min {};
    IVec3 max {};
};

class Bounds2
{
public:
    Bounds2() = default;
    Bounds2(const Vec2& min, const Vec2& max);
    explicit Bounds2(const IBounds2& bounds);

    Vec2 min {};
    Vec2 max {};
};

class Bounds3
{
public:
    Bounds3() = default;
    Bounds3(const Vec3& min, const Vec3& max);
    explicit Bounds3(const IBounds3& bounds);

    Vec3 min {};
    Vec3 max {};
};

constexpr f32
to_radians(const f32 degrees)
{
    return degrees * std::numbers::pi_v<f32> / 180.0f;
}

constexpr f32
to_degrees(const f32 radians)
{
    return radians * 180.0f / std::numbers::pi_v<f32>;
}

Mat4 get_view_matrix(const Vec3& position, const Vec3& rotation);
Mat4 get_orthographic_matrix(const Vec2& min, const Vec2& max, f32 near, f32 far);
Mat4 get_projection_matrix(f32 fov_y, f32 aspect, f32 near, f32 far);
Mat4 get_look_at_matrix(const Vec3& position, const Vec3& target, const Vec3& up);

Vec3 get_forward(const Vec3& rotation);
Vec3 get_right(const Vec3& rotation);
Vec3 get_up(const Vec3& rotation);

f32 interpolate_to(f32 current, f32 target, f32 speed, f32 delta_time);
Vec3 interpolate_to(const Vec3& current, const Vec3& target, f32 speed, f32 delta_time);

Vec2 direction_from_angle(f32 rotation_degrees);

constexpr Direction
get_direction_opposite(const Direction& direction)
{
    switch (direction)
    {
        case Direction::East:   return Direction::West;
        case Direction::West:   return Direction::East;
        case Direction::North:  return Direction::South;
        case Direction::South:  return Direction::North;
        case Direction::Up:     return Direction::Down;
        case Direction::Down:   return Direction::Up;
        default:                throw std::invalid_argument("invalid direction");
    }
}

constexpr Vec3
get_direction_normal(const Direction& direction)
{
    const s32 direction_index { 3 * static_cast<s32>(direction) };

    return {
        DIRECTION_NORMAL_ARRAY[direction_index + 0],
        DIRECTION_NORMAL_ARRAY[direction_index + 1],
        DIRECTION_NORMAL_ARRAY[direction_index + 2]
    };
}

constexpr std::string_view
get_direction_string(Direction direction)
{
    switch (direction)
    {
        case Direction::East:   return "West";
        case Direction::West:   return "East";
        case Direction::North:  return "South";
        case Direction::South:  return "North";
        case Direction::Up:     return "Down";
        case Direction::Down:   return "Up";
        default:                throw std::invalid_argument("invalid direction");
    }
}

constexpr Direction
get_direction_from_mask(const u8 mask)
{
    if (mask == 0)
    {
        throw std::invalid_argument("empty direction mask");
    }

    const s32 index { __builtin_ctz(static_cast<unsigned>(mask)) };

    if (index >= DIRECTION_COUNT)
    {
        throw std::invalid_argument("invalid direction mask");
    }

    return static_cast<Direction>(index);
}

IVec2 rotate_point(IVec2 point, IVec2 pivot, Direction direction);
