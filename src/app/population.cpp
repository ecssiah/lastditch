#include "app/population.h"

#include "core/types.h"
#include "core/log.h"
#include "app/actor.h"
#include "app/nation.h"
#include "app/physics.h"
#include "app/work.h"
#include "app/world.h"

Population::Population()
    :
    random{POPULATION_SEED},
    nation_array{},
    judge_id{},
    actor_pool{}
{

}

void
Population::init(Work& work)
{
    init_nations();

    init_actor_pool();

    init_judge();
    init_agents(work);
}

void
Population::quit()
{

}

Actor&
Population::get_judge()
{
    return actor_pool.actor_array[judge_id];
}

const Actor&
Population::get_judge() const
{
    return actor_pool.actor_array[judge_id];
}

Actor&
Population::get_actor(s32 actor_id)
{
    return actor_pool.actor_array[actor_id];
}

const Actor&
Population::get_actor(s32 actor_id) const
{
    return actor_pool.actor_array[actor_id];
}

void
Population::init_judge()
{
    Actor judge{};
    judge.actor_type = ActorType::Judge;
    judge.nation_type = NationType::Lion;
    judge.movement_type = MovementType::Ground;
    judge.position = {WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f};
    judge.rotation = {0.0f, 0.0f, 90.0f};
    judge.rotation_target = {0.0f, 0.0f, 90.0f};
    judge.is_grounded = false;
    judge.speed = JUDGE_DEFAULT_GROUND_SPEED;
    judge.velocity = {0.0f, 0.0f, 0.0f};
    judge.box_collider = {
        .collision_enabled = true,
        .radius = {0.30f, 0.30f, 0.90f},
    };

    actor_add(actor_pool, judge);

    judge_id = judge.actor_id;

    LOG_INFO(
        "Generated %s judge, ID: %i, at (%.1f %.1f %.1f)", 
        nation_type_string_array[static_cast<u8>(judge.nation_type)],
        judge_id,
        judge.position.x, 
        judge.position.y, 
        judge.position.z
    );
}

void
Population::init_agents(Work& work)
{
    for (s32 nation_index = 0; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (s32 agent_index = 0; agent_index < AGENT_INITIAL_COUNT; ++agent_index)
        {
            const s32 nation_type_index = random.uniform(0, NATION_TYPE_COUNT - 1);
            
            const auto nation_type = static_cast<NationType>(nation_type_index);
            const Nation& nation = nation_array[nation_type_index];

            const Vec3 position{
                static_cast<f32>(nation.home_coordinate.x - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.y - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.z + 4),
            };

            const Vec3 rotation = {
                0.0f, 
                0.0f, 
                static_cast<f32>(random.uniform(0, 360))
            };

            Actor agent{};
            agent.actor_type = ActorType::Agent;
            agent.nation_type = nation_type;
            agent.movement_type = MovementType::Ground;
            agent.position = position;
            agent.rotation = rotation;
            agent.rotation_target = rotation;
            agent.is_grounded = false;
            agent.speed = AGENT_DEFAULT_GROUND_SPEED;
            agent.velocity = vec3_broadcast(0.0f);
            agent.box_collider = {
                .collision_enabled = true,
                .radius = {0.40f, 0.40f, 0.90f},
            };

            actor_add(actor_pool, agent);

            TaskState act_state{};
            act_state.wander = {
                .tick = random.uniform(0, 500),
                .tick_limit = 500,
            };

            work_add_task(work, agent, TaskType::wander, act_state);

            LOG_INFO(
                "Generated %s agent, ID: %i, at (%.1f %.1f %.1f)",
                nation_type_string_array[nation_type_index],
                agent.actor_id,
                agent.position.x, 
                agent.position.y, 
                agent.position.z
            );
        }
    }
}

void 
Population::init_nations()
{
    constexpr f32 nation_offset = 76.0f;

    Nation& wolf_nation = nation_array[static_cast<u8>(NationType::Wolf)];

    wolf_nation.nation_type = NationType::Wolf;

    wolf_nation.home_coordinate.x = WORLD_CENTER_F32 + nation_offset;
    wolf_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    wolf_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& eagle_nation = nation_array[static_cast<u8>(NationType::Eagle)];

    eagle_nation.nation_type = NationType::Eagle;

    eagle_nation.home_coordinate.x = WORLD_CENTER_F32 - nation_offset;
    eagle_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    eagle_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& bear_nation = nation_array[static_cast<u8>(NationType::Bear)];

    bear_nation.nation_type = NationType::Bear;

    bear_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    bear_nation.home_coordinate.y = WORLD_CENTER_F32 + nation_offset;
    bear_nation.home_coordinate.z = ROOF_Z + 1.0f;

    Nation& lion_nation = nation_array[static_cast<u8>(NationType::Lion)];

    lion_nation.nation_type = NationType::Lion;

    lion_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    lion_nation.home_coordinate.y = WORLD_CENTER_F32 - nation_offset;
    lion_nation.home_coordinate.z = ROOF_Z + 3.0f;
}

void
Population::init_actor_pool()
{
    actor_pool.free_count = ACTION_MAX;
    actor_pool.active_count = 0;

    for (s32 pool_id = 0; pool_id < ACTION_MAX; ++pool_id)
    {
        actor_pool.active_array[pool_id] = 0;
        actor_pool.free_array[pool_id] = pool_id;
    }
}
