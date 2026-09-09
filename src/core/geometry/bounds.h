#pragma once

#include <algorithm>
#include <array>
#include <cstddef>
#include "vector.h"
#include "core/types.h"

class IBounds2
{
public:
    constexpr
    IBounds2() = default;

    constexpr
    IBounds2(const IVec2& min, const IVec2& max)
        :
        min { min },
        max { max }
    {

    }

    constexpr IVec2
    position() const
    {
        return
        {
            min.x,
            min.y
        };
    }

    constexpr IVec2
    size() const
    {
        return
        {
            max.x - min.x,
            max.y - min.y,
        };
    }

    IVec2 min {};
    IVec2 max {};
};

struct IBounds2List
{
    std::array<IBounds2, 4> values {};
    std::size_t count {};

    constexpr auto
    begin() const
    {
        return values.begin();
    }

    constexpr auto
    end() const
    {
        return values.begin() + count;
    }

    constexpr void
    push_back(const IBounds2& bounds)
    {
        values[count++] = bounds;
    }
};

class IBounds3
{
public:
    constexpr
    IBounds3() = default;

    constexpr
    IBounds3(const IVec3& min, const IVec3& max)
        :
        min { min },
        max { max }
    {

    }

    constexpr IVec3
    position() const
    {
        return
        {
            min.x,
            min.y,
            min.z,
        };
    }

    constexpr IVec3
    size() const
    {
        return
        {
            max.x - min.x,
            max.y - min.y,
            max.z - min.z,
        };
    }

    IVec3 min {};
    IVec3 max {};
};

class Bounds2
{
public:
    constexpr
    Bounds2() = default;

    constexpr explicit
    Bounds2(const IBounds2& bounds)
        :
        min { bounds.min },
        max { bounds.max }
    {

    }

    constexpr
    Bounds2(const Vec2& min, const Vec2& max)
        :
        min { min },
        max { max }
    {

    }

    Vec2 min {};
    Vec2 max {};
};

class Bounds3
{
public:
    constexpr
    Bounds3() = default;

    constexpr explicit
    Bounds3(const IBounds3& bounds)
        :
        min { bounds.min },
        max { bounds.max }
    {

    }

    constexpr
    Bounds3(const Vec3 &min, const Vec3 &max)
        :
        min { min },
        max { max }
    {

    }

    Vec3 min {};
    Vec3 max {};
};

constexpr b32
overlaps(const IBounds2& left, const IBounds2& right)
{
    const b32 is_clear
    {
        left.max.x <= right.min.x ||
        right.max.x <= left.min.x ||
        left.max.y <= right.min.y ||
        right.max.y <= left.min.y
    };

    return !is_clear;
}

constexpr IBounds2
get_intersection(const IBounds2& left, const IBounds2& right)
{
    const IBounds2 bounds
    {
        {
            std::max(left.min.x, right.min.x),
            std::max(left.min.y, right.min.y)
        },
        {
            std::min(left.max.x, right.max.x),
            std::min(left.max.y, right.max.y)
        },
    };

    return bounds;
}

constexpr IBounds2List
subtract(const IBounds2& left, const IBounds2& right)
{
    IBounds2List bounds_list {};

    if (!overlaps(left, right))
    {
        bounds_list.push_back(left);

        return bounds_list;
    }

    const IBounds2 intersection_bounds { get_intersection(left, right) };

    if (intersection_bounds.min.x > left.min.x)
    {
        const IBounds2 bounds
        {
            { left.min.x, left.min.y },
            { intersection_bounds.min.x, left.max.y }
        };

        bounds_list.push_back(bounds);
    }

    if (intersection_bounds.max.x < left.max.x)
    {
        const IBounds2 bounds
        {
            { intersection_bounds.max.x, left.min.y },
            { left.max.x, left.max.y}
        };

        bounds_list.push_back(bounds);
    }

    if (intersection_bounds.min.y > left.min.y)
    {
        const IBounds2 bounds
        {
            { intersection_bounds.min.x, left.min.y },
            { intersection_bounds.max.x, intersection_bounds.min.y }
        };

        bounds_list.push_back(bounds);
    }

    if (intersection_bounds.max.y < left.max.y)
    {
        const IBounds2 bounds
        {
            { intersection_bounds.min.x, intersection_bounds.max.y },
            { intersection_bounds.max.x, left.max.y }
        };

        bounds_list.push_back(bounds);
    }

    return bounds_list;
}
