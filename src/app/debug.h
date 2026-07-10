#pragma once

#include "core/types.h"
#include "core/geometry.h"

constexpr s32 DEBUG_LINE_MAX {1 << 13};

constexpr b32 DEBUG_AREAS {true};
constexpr s32 DEBUG_FLOOR_NUMBER {6};

constexpr Vec3 COLOR_RED    {1.0f, 0.0f, 0.0f};
constexpr Vec3 COLOR_GREEN  {0.0f, 1.0f, 0.0f};
constexpr Vec3 COLOR_BLUE   {0.0f, 0.0f, 1.0f};

class DebugLine
{
public:
    Vec3 a;
    Vec3 b;
    Vec3 color;
};

class Debug
{
public:
    void init();
    void quit();
    void reset();

    void add_line(const Vec3& a, const Vec3& b, const Vec3& color);
    void add_box(const Vec3& min, const Vec3& max, const Vec3& color);

    [[nodiscard]] const std::vector<DebugLine>& get_debug_line_vector() const;

private:
    std::vector<DebugLine> line_vector;
};
