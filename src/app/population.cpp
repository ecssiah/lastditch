#include "population.h"

#include "actor.h"
#include "physics.h"
#include "world.h"
#include "work.h"
#include "core/log.h"
#include "core/types.h"

void
Population::init(Work& work)
{
    actor_vector.reserve(ACTOR_POOL_MAX);

    init_nations();
    init_judge();
    init_agents(work);

    LOG_INFO("POPULATION INIT");
}

void
Population::update(World& world)
{
    for (Actor& actor : actor_vector)
    {
        Physics::update_actor(world, actor);
    }
}

void
Population::quit()
{
    LOG_INFO("POPULATION QUIT");
}

Random&
Population::get_random()
{
    return random;
}

Actor&
Population::get_actor(const s32 actor_id)
{
    return actor_vector[actor_id];
}

const Actor&
Population::get_actor(const s32 actor_id) const
{
    return actor_vector[actor_id];
}

vector<Actor>&
Population::get_actor_vector()
{
    return actor_vector;
}

const vector<Actor>&
Population::get_actor_vector() const
{
    return actor_vector;
}

void
Population::init_judge()
{
    judge_id = actor_id_generator.next();

    const Actor judge {
        .id = judge_id,
        .actor_type = ActorType::Judge,
        .nation_type = NationType::Lion,
        .position = { WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f },
        .rotation = { 0.0f, 0.0f, 90.0f },
        .rotation_target = { 0.0f, 0.0f, 90.0f },
        .speed = JUDGE_DEFAULT_GROUND_SPEED,
        .velocity = { 0.0f, 0.0f, 0.0f },
        .box_collider = {
            .collision_enabled = true,
            .radius = { 0.30f, 0.30f, 0.90f },
        },
    };

    actor_vector.push_back(judge);

    LOG_INFO(
        "Generated %s judge, ID: %i, at (%.1f %.1f %.1f)",
        get_nation_type_string(judge.nation_type),
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
            const s32 nation_type_index { random.uniform(0, NATION_TYPE_COUNT - 1) };
            
            const auto nation_type { static_cast<NationType>(nation_type_index) };
            const Nation& nation { nation_array[nation_type_index] };

            const Vec3 position {
                static_cast<f32>(nation.home_coordinate.x - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.y - 6 + random.uniform(0, 11)),
                static_cast<f32>(nation.home_coordinate.z + 4),
            };

            const Vec3 rotation {
                0.0f, 
                0.0f, 
                static_cast<f32>(random.uniform(0, 360))
            };

            const Actor agent {
                .id = actor_id_generator.next(),
                .actor_type = ActorType::Agent,
                .nation_type = nation_type,
                .position = position,
                .rotation = rotation,
                .rotation_target = rotation,
                .speed = AGENT_DEFAULT_GROUND_SPEED,
                .box_collider = {
                    .collision_enabled = true,
                    .radius = { 0.40f, 0.40f, 0.90f },
                },
            };

            actor_vector.push_back(agent);

            LOG_INFO(
                "Generated %s agent, ID: %i, at (%.1f %.1f %.1f)",
                get_nation_type_string(nation_type),
                agent.id,
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
    constexpr f32 nation_offset { 76.0f };

    Nation& wolf_nation { nation_array[static_cast<u8>(NationType::Wolf)] };

    wolf_nation.nation_type = NationType::Wolf;

    wolf_nation.home_coordinate.x = WORLD_CENTER_F32 + nation_offset;
    wolf_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    wolf_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& eagle_nation { nation_array[static_cast<u8>(NationType::Eagle)] };

    eagle_nation.nation_type = NationType::Eagle;

    eagle_nation.home_coordinate.x = WORLD_CENTER_F32 - nation_offset;
    eagle_nation.home_coordinate.y = WORLD_CENTER_F32 + 0.0f;
    eagle_nation.home_coordinate.z = ROOF_Z + 3.0f;

    Nation& bear_nation { nation_array[static_cast<u8>(NationType::Bear)] };

    bear_nation.nation_type = NationType::Bear;

    bear_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    bear_nation.home_coordinate.y = WORLD_CENTER_F32 + nation_offset;
    bear_nation.home_coordinate.z = ROOF_Z + 1.0f;

    Nation& lion_nation { nation_array[static_cast<u8>(NationType::Lion)] };

    lion_nation.nation_type = NationType::Lion;

    lion_nation.home_coordinate.x = WORLD_CENTER_F32 + 0.0f;
    lion_nation.home_coordinate.y = WORLD_CENTER_F32 - nation_offset;
    lion_nation.home_coordinate.z = ROOF_Z + 3.0f;
}
