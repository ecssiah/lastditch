#pragma once

#include "nation.h"
#include "core/id.h"
#include "core/random.h"

class Actor;
class Work;
class World;

constexpr u32 POPULATION_SEED { 1388 };
constexpr s32 AGENT_INITIAL_COUNT { 12 };

class Population
{
public:
    void init(Work& work);
    void update(World& world);

    Random& get_random();

    Actor& get_actor(s32 actor_id);
    [[nodiscard]] const Actor& get_actor(s32 actor_id) const;

    std::vector<Actor>& get_actor_vector();
    [[nodiscard]] const vector<Actor>& get_actor_vector() const;

    s32 judge_id { -1 };

private:
    void init_judge();
    void init_agents(Work& work);
    void init_nations();

    IdGenerator actor_id_generator {};
    IdGenerator task_id_generator {};

    Random random { POPULATION_SEED };

    std::array<Nation, NATION_TYPE_COUNT> nation_array {};

    std::vector<Actor> actor_vector {};
};
