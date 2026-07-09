#pragma once

#include <functional>
#include <utility>
#include "app/actor.h"
#include "app/nation.h"
#include "app/work.h"
#include "core/random.h"

constexpr u32 POPULATION_SEED {1388};
constexpr s32 AGENT_INITIAL_COUNT {12};

class Population
{
public:
    Population();

    void init(Work& work);
    void quit();

    Actor& get_judge();

    [[nodiscard]]
    const Actor& get_judge() const;

    Actor& get_actor(s32 actor_id);

    [[nodiscard]]
    const Actor& get_actor(s32 actor_id) const;

    [[nodiscard]]
    ActorPool& get_actor_pool();

private:
    void init_judge();
    void init_agents(Work& work);
    void init_nations();

    Random random;

    std::array<Nation, NATION_TYPE_COUNT> nation_array;
    s32 judge_id;
    ActorPool actor_pool;
};
