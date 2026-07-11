#pragma once

#include "app/actor.h"
#include "app/nation.h"
#include "app/work.h"
#include "core/id.h"
#include "core/random.h"

constexpr u32 POPULATION_SEED { 1388 };
constexpr s32 AGENT_INITIAL_COUNT { 12 };

class Population
{
public:
    void init();
    void update(f32 delta_time);
    void quit();

    Random& get_random();

    Actor& get_actor(s32 actor_id);
    [[nodiscard]] const Actor& get_actor(s32 actor_id) const;

    std::vector<Actor>& get_actor_vector();
    [[nodiscard]] const vector<Actor>& get_actor_vector() const;

    void add_act(const Act& act);

    s32 judge_id { -1 };

private:
    void init_judge();
    void init_agents();
    void init_nations();

    IdGenerator actor_id_generator {};
    IdGenerator task_id_generator {};

    Random random { POPULATION_SEED };
    Work work {};

    std::array<Nation, NATION_TYPE_COUNT> nation_array {};

    std::vector<Actor> actor_vector {};
};
