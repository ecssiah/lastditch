#include "population.h"

#include "actor.h"
#include "app.h"
#include "physics.h"
#include "world.h"
#include "work.h"
#include "core/log.h"
#include "core/types.h"

void
Population::init()
{
    actor_vector.reserve(INITIAL_POPULATION_CAPACITY);

    init_judge();
    init_agents();

    LOG_INFO("POPULATION INIT");
}

void
Population::update(World& world, Work& work)
{
    for (Actor& actor : actor_vector)
    {
        actor.rotation = interpolate_to(
            actor.rotation,
            actor.rotation_target,
            12.0f,
            FIXED_FRAME_TIME_32
        );

        Physics::update_actor(world, actor);
    }
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

Nation&
Population::get_nation(NationType nation_type)
{
    const u8 nation_index { static_cast<u8>(nation_type) };

    return nation_array[nation_index];
}

void
Population::init_judge()
{
    judge_id = actor_id_generator.next();

    const Actor judge {
        .id = judge_id,
        .actor_type = ActorType::Judge,
        .nation_type = NationType::Lion,
        .move_speed = ACTOR_DEFAULT_MOVE_SPEED,
        .position = { WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f },
        .rotation = { 0.0f, 0.0f, 90.0f },
        .velocity = { 0.0f, 0.0f, 0.0f },
        .position_target = { WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, ROOF_Z + 4.0f },
        .rotation_target = { 0.0f, 0.0f, 90.0f },
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
Population::init_agents()
{
    for (s32 nation_index { 0 }; nation_index < NATION_TYPE_COUNT; ++nation_index)
    {
        for (s32 agent_index { 0 }; agent_index < INITIAL_POPULATION_SIZE; ++agent_index)
        {
            const s32 nation_type_index { random.uniform(0, NATION_TYPE_COUNT - 1) };
            
            const auto nation_type { static_cast<NationType>(nation_type_index) };
            const Nation& nation { nation_array[nation_type_index] };

            const IVec3 position {
                nation.home_coordinate.x - 6 + random.uniform(0, 11),
                nation.home_coordinate.y - 6 + random.uniform(0, 11),
                nation.home_coordinate.z + 4,
            };

            const IVec3 rotation {
                0,
                0,
                random.uniform(0, 360)
            };

            const Actor agent {
                .id = actor_id_generator.next(),
                .actor_type = ActorType::Agent,
                .nation_type = nation_type,
                .move_speed = ACTOR_DEFAULT_MOVE_SPEED,
                .position = Vec3 { position },
                .rotation = Vec3 { rotation },
                .velocity = {},
                .position_target = Vec3 { position },
                .rotation_target = Vec3 { rotation },
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
