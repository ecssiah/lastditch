#pragma once

#include <vector>

#include "vector.h"
#include "core/types.h"

class IBounds2
{
public:
    IBounds2() = default;

    IBounds2(const IVec2& min, const IVec2& max)
        :
        min { min },
        max { max }
    {

    }

    IVec2
    position() const
    {
        return {
            min.x,
            min.y
        };
    }

    IVec2
    size() const
    {
        return {
            max.x - min.x,
            max.y - min.y,
        };
    }

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

    IBounds3(const IVec3& min, const IVec3& max)
        :
        min { min },
        max { max }
    {

    }

    IVec3
    position() const
    {
        return {
            min.x,
            min.y,
            min.z,
        };
    }

    IVec3
    size() const
    {
        return {
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
    Bounds2() = default;

    explicit
    Bounds2(const IBounds2& bounds)
        :
        min { bounds.min },
        max { bounds.max }
    {

    }

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
    Bounds3() = default;

    explicit
    Bounds3(const IBounds3& bounds)
        :
        min { bounds.min },
        max { bounds.max }
    {

    }

    Bounds3(const Vec3 &min, const Vec3 &max)
        :
        min { min },
        max { max }
    {

    }

    Vec3 min {};
    Vec3 max {};
};

inline b32
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

inline IBounds2
get_intersection(const IBounds2& lhs, const IBounds2& rhs)
{
    const IBounds2 bounds {
        {
            std::max(lhs.min.x, rhs.min.x),
            std::max(lhs.min.y, rhs.min.y)
        },
        {
            std::min(lhs.max.x, rhs.max.x),
            std::min(lhs.max.y, rhs.max.y)
        },
    };

    return bounds;
}

inline std::vector<IBounds2>
subtract(const IBounds2& lhs, const IBounds2& rhs)
{
    if (!overlaps(lhs, rhs))
    {
        return { lhs };
    }

    std::vector<IBounds2> bounds_vector {};
    const IBounds2 intersection_bounds { get_intersection(lhs, rhs) };

    if (intersection_bounds.min.x > lhs.min.x)
    {
        const IBounds2 bounds {
            { lhs.min.x, lhs.min.y },
            { intersection_bounds.min.x, lhs.max.y }
        };

        bounds_vector.push_back(bounds);
    }

    if (intersection_bounds.max.x < lhs.max.x)
    {
        const IBounds2 bounds {
            { intersection_bounds.max.x, lhs.min.y },
            { lhs.max.x, lhs.max.y}
        };

        bounds_vector.push_back(bounds);
    }

    if (intersection_bounds.min.y > lhs.min.y)
    {
        const IBounds2 bounds {
            { intersection_bounds.min.x, lhs.min.y },
            { intersection_bounds.max.x, intersection_bounds.min.y }
        };

        bounds_vector.push_back(bounds);
    }

    if (intersection_bounds.max.y < lhs.max.y)
    {
        const IBounds2 bounds {
            { intersection_bounds.min.x, intersection_bounds.max.y },
            { intersection_bounds.max.x, lhs.max.y }
        };

        bounds_vector.push_back(bounds);
    }

    return bounds_vector;
}
