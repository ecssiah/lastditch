#pragma once

#include "nation.h"
#include "world.h"
#include "core/id.h"
#include "core/random.h"

class Actor;
class Work;
class World;

class Population
{
public:
    void init();
    void update(World& world, Work& work);

    Actor& get_actor(s32 actor_id);
    [[nodiscard]] const Actor& get_actor(s32 actor_id) const;

    std::vector<Actor>& get_actor_vector();
    [[nodiscard]] const vector<Actor>& get_actor_vector() const;

    Nation& get_nation(NationType nation_type);

    Random random { POPULATION_SEED };

    s32 judge_id { -1 };

private:
    void init_judge();
    void init_agents();

    IdGenerator actor_id_generator {};
    IdGenerator task_id_generator {};

    std::array<Nation, NATION_TYPE_COUNT> nation_array {
        Nation {
            .nation_type = NationType::Wolf,
            .home_coordinate = {
                WORLD_CENTER_S32 + NATION_HOME_OFFSET,
                WORLD_CENTER_S32,
                ROOF_Z + 3,
            },
        },
        Nation {
            .nation_type = NationType::Eagle,
            .home_coordinate = {
                WORLD_CENTER_S32 - NATION_HOME_OFFSET,
                WORLD_CENTER_S32,
                ROOF_Z + 3,
            },
        },
        Nation {
            .nation_type = NationType::Lion,
            .home_coordinate = {
                WORLD_CENTER_S32,
                WORLD_CENTER_S32 - NATION_HOME_OFFSET,
                ROOF_Z + 3,
            },
        },
        Nation {
            .nation_type = NationType::Bear,
            .home_coordinate = {
                WORLD_CENTER_S32,
                WORLD_CENTER_S32 + NATION_HOME_OFFSET,
                ROOF_Z + 3,
            },
        },
    };

    std::vector<Actor> actor_vector {};
};
