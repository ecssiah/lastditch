#include "debug.h"

#include "world.h"

void
Debug::init(const World& world)
{
    if (DEBUG_SHOW_AREAS)
    {
        for (const s32 floor_number : DEBUG_FLOOR_ARRAY)
        {
            const vector<Area>& area_vector { world.floor_area_vector[floor_number] };

            for (const Area& area : area_vector)
            {
                const IBounds3 area_bounds {
                    {
                        area.bounds.min.x,
                        area.bounds.min.y,
                        area.floor_number * FLOOR_SIZE_Z
                    },
                    {
                        area.bounds.max.x,
                        area.bounds.max.y,
                        area.floor_number * FLOOR_SIZE_Z + 2
                    },
                };

                add_box(Vec3 { area_bounds.min }, Vec3 { area_bounds.max }, Color::Red);

                for (const s32 border_id : area.border_id_vector)
                {
                    const Border& border { world.border_vector[border_id] };

                    for (const Link& link : border.link_vector)
                    {
                        const Vec3 link_position { link.position };

                        if (link.axis == Axis::X)
                        {
                            const Bounds3 link_bounds {
                                { link_position.x, link_position.y, link_position.z + 1 },
                                { link_position.x + 1, link_position.y + 2, link_position.z + 3 },
                            };

                            add_box(link_bounds.min, link_bounds.max, Color::Cyan);
                        }
                        else if (link.axis == Axis::Y)
                        {
                            const Bounds3 link_bounds {
                                { link_position.x, link_position.y, link_position.z + 1 },
                                { link_position.x + 2, link_position.y + 1, link_position.z + 3 },
                            };

                            add_box(link_bounds.min, link_bounds.max, Color::Cyan);
                        }
                    }
                }
            }
        }
    }
}

void
Debug::update()
{

}

void 
Debug::add_line(const Vec3& a, const Vec3& b, const Color& color)
{
    const DebugLine debug_line { a, b, color };
    
    line_vector.push_back(debug_line);
}

void 
Debug::add_box(const Vec3& min, const Vec3& max, const Color& color)
{
    const Vec3 v000 { min.x, min.y, min.z };
    const Vec3 v100 { max.x, min.y, min.z };
    const Vec3 v010 { min.x, max.y, min.z };
    const Vec3 v110 { max.x, max.y, min.z };

    const Vec3 v001 { min.x, min.y, max.z };
    const Vec3 v101 { max.x, min.y, max.z };
    const Vec3 v011 { min.x, max.y, max.z };
    const Vec3 v111 { max.x, max.y, max.z };

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
