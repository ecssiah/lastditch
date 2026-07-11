#include "debug.h"

#include "world.h"

void 
Debug::add_line(const Vec3& a, const Vec3& b, const Vec3& color)
{
    const DebugLine debug_line{a, b, color};
    
    line_vector.push_back(debug_line);
}

void 
Debug::add_box(const Vec3& min, const Vec3& max, const Vec3& color)
{
    const Vec3 v000{min.x, min.y, min.z};
    const Vec3 v100{max.x, min.y, min.z};
    const Vec3 v010{min.x, max.y, min.z};
    const Vec3 v110{max.x, max.y, min.z};

    const Vec3 v001{min.x, min.y, max.z};
    const Vec3 v101{max.x, min.y, max.z};
    const Vec3 v011{min.x, max.y, max.z};
    const Vec3 v111{max.x, max.y, max.z};

    add_line(v000, v100, color);
    add_line(v100, v110, color);
    add_line(v110, v010, color);
    add_line(v010, v000, color);

    add_line(v001, v101, color);
    add_line(v101, v111, color);
    add_line(v111, v011, color);
    add_line(v011, v001, color);

    add_line(v000, v001, color);
    add_line(v100, v101, color);
    add_line(v110, v111, color);
    add_line(v010, v011, color);
}

const std::vector<DebugLine>&
Debug::get_debug_line_vector() const
{
    return line_vector;
}

void 
Debug::reset()
{
    line_vector.clear();
}

void 
Debug::init(const World& world)
{
    const vector<Area>& floor_area_vector {world.get_floor_area_vector(DEBUG_FLOOR_NUMBER)};

    for (const Area& area : floor_area_vector)
    {
        const Vec3 area_min {
            static_cast<f32>(area.bounds.min.x),
            static_cast<f32>(area.bounds.min.y),
            static_cast<f32>(area.floor_number * FLOOR_SIZE_Z)
        };

        const Vec3 area_max {
            static_cast<f32>(area.bounds.max.x),
            static_cast<f32>(area.bounds.max.y),
            static_cast<f32>(area.floor_number * FLOOR_SIZE_Z) + 2.0f
        };

        add_box(area_min, area_max, COLOR_RED);

        // for (const s32 edge_id : area.edge_id_vector)
        // {
        //     const Edge& area_edge {world.get_edge_vector()[edge_id]};
        //
        //     const IVec2 area_overlap_bounds_size {area_edge.area_overlap.bounds.size()};
        //
        //     const IVec3 door_position {
        //         area_edge.area_overlap.bounds.min.x + area_overlap_bounds_size.x / 2,
        //         area_edge.area_overlap.bounds.min.y + area_overlap_bounds_size.y / 2,
        //         area.floor_number * FLOOR_SIZE_Z + 1,
        //     };
        //
        //     const Vec3 edge_debug_min {Vec3{door_position}};
        //     const Vec3 edge_debug_max {Vec3{door_position + IVec3{1}}};
        //
        //     add_box(edge_debug_min, edge_debug_max, COLOR_GREEN);
        // }
    }
}

void
Debug::update()
{

}
