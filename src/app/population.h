#pragma once

#include <functional>
#include <utility>
#include "app/actor.h"
#include "app/nation.h"
#include "app/work.h"
#include "core/id.h"
#include "core/random.h"

constexpr u32 POPULATION_SEED {1388};
constexpr s32 AGENT_INITIAL_COUNT {12};

class Population
{
public:
    void init(Work& work);
    void quit();

    Actor& get_judge();
    [[nodiscard]] const Actor& get_judge() const;

    Actor& get_actor(s32 actor_id);
    [[nodiscard]] const Actor& get_actor(s32 actor_id) const;

    [[nodiscard]] std::vector<Actor>& get_actor_vector();

private:
    void init_judge();
    void init_agents(Work& work);
    void init_nations();

    IdGenerator actor_id_generator {};
    IdGenerator task_id_generator {};

    Random random {POPULATION_SEED};

    std::array<Nation, NATION_TYPE_COUNT> nation_array {};
    s32 judge_id {-1};

    std::vector<Actor> actor_vector {};
};
