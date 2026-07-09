#pragma once

#include <functional>
#include <utility>
#include "app/actor.h"
#include "app/nation.h"
#include "app/work.h"
#include "core/random.h"

constexpr u32 POPULATION_SEED {1388};
constexpr s32 AGENT_INITIAL_COUNT {12};

struct ActorPool
{
    s32 active_count;
    std::array<s32, ACTION_MAX> active_array;
    std::array<s32, ACTION_MAX> active_lookup;

    s32 free_count;
    std::array<s32, ACTION_MAX> free_array;

    std::array<Actor, ACTION_MAX> actor_array;
};

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

    template <typename Func>
    void for_each_active_actor(Func&& func)
    {
        for (s32 pool_id = 0; pool_id < actor_pool.active_count; ++pool_id)
        {
            const s32 actor_id = actor_pool.active_array[pool_id];
            Actor& actor = actor_pool.actor_array[actor_id];

            std::invoke(std::forward<Func>(func), actor);
        }
    }

    template <typename Func>
    void for_each_active_actor(Func&& func) const
    {
        for (s32 pool_id = 0; pool_id < actor_pool.active_count; ++pool_id)
        {
            const s32 actor_id = actor_pool.active_array[pool_id];
            const Actor& actor = actor_pool.actor_array[actor_id];

            std::invoke(std::forward<Func>(func), actor);
        }
    }

private:
    void init_judge();
    void init_agents(Work& work);
    void init_nations();
    void init_actor_pool();

    void add_actor(Actor& actor);

    Random random;

    std::array<Nation, NATION_TYPE_COUNT> nation_array;
    s32 judge_id;
    ActorPool actor_pool;
};
