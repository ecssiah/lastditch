// ============================================================================
// File:    src/app/population.c
// Layer:   app
// Purpose: Implementation of the population database declared in
//          app/population.h.
// ============================================================================

#include "app/population.h"

#include "core/geometry.h"
#include "core/log.h"

// ----------------------------------------------------------------------------
// Function: population_init_judge
// Purpose:  Inserts the judge, the actor the player embodies, just south of
//           the world center above the roof.
// Params:   population - population to extend
// Returns:  nothing
// Reads:    NATION_TYPE_NAME
// Writes:   population.actor, population.judge_id
// ----------------------------------------------------------------------------
static void
population_init_judge(Population* population)
{
    ActorRecord judge;

    judge.actor_id = sequence_next(&population->actor.id_sequence);
    judge.actor_type = ACTOR_TYPE_JUDGE;
    judge.nation_type = NATION_TYPE_LION;
    judge.move_speed = JUDGE_DEFAULT_MOVE_SPEED;
    judge.turn_speed = JUDGE_DEFAULT_TURN_SPEED;
    judge.position = vec3_make(WORLD_CENTER_F32, WORLD_CENTER_F32 - 12.0f, (f32)ROOF_Z + 4.0f);
    judge.position_target = judge.position;
    judge.orientation = quaternion_from_euler_degrees(0.0f, 0.0f, 90.0f);
    judge.rotation_target = vec3_make(0.0f, 0.0f, 90.0f);
    judge.movement_yaw = 90.0f;
    judge.velocity = vec3_make(0.0f, 0.0f, 0.0f);
    judge.collision_enabled = true;
    judge.collider_radius = vec3_make(0.30f, 0.30f, 0.90f);
    judge.is_grounded = false;

    population->judge_id = judge.actor_id;

    actor_table_insert(&population->actor, &judge);

    LOG_INFO(
        "Generated %s judge, ID: %i, at (%.1f %.1f %.1f)",
        NATION_TYPE_NAME[judge.nation_type],
        (int)judge.actor_id,
        (double)judge.position.x,
        (double)judge.position.y,
        (double)judge.position.z
    );
}

// ----------------------------------------------------------------------------
// Function: population_init_agents
// Purpose:  Inserts the agents of every nation at random cells around the
//           nation's home, facing random headings, and registers each in
//           the decision table.
// Params:   population - population to extend
// Returns:  nothing
// Reads:    NATION_TABLE, NATION_TYPE_NAME
// Writes:   population.actor, population.decision, population.random
// ----------------------------------------------------------------------------
static void
population_init_agents(Population* population)
{
    s32 nation_row;
    s32 agent_index;

    for (nation_row = 0; nation_row < NATION_TYPE_COUNT; ++nation_row)
    {
        const NationRecord* nation = &NATION_TABLE[nation_row];

        for (agent_index = 0; agent_index < INITIAL_POPULATION_SIZE; ++agent_index)
        {
            ActorRecord agent;
            IVec3 position;
            IVec3 rotation;
            Vec3 rotation_degrees;

            position.x = nation->home_coordinate.x - 6 + random_uniform_s32(&population->random, 0, 11);
            position.y = nation->home_coordinate.y - 6 + random_uniform_s32(&population->random, 0, 11);
            position.z = nation->home_coordinate.z + 4;

            rotation.x = 0;
            rotation.y = 0;
            rotation.z = random_uniform_s32(&population->random, 0, 360);

            rotation_degrees = vec3_from_ivec3(rotation);

            agent.actor_id = sequence_next(&population->actor.id_sequence);
            agent.actor_type = ACTOR_TYPE_AGENT;
            agent.nation_type = nation->nation_type;
            agent.move_speed = ACTOR_DEFAULT_MOVE_SPEED;
            agent.turn_speed = ACTOR_DEFAULT_TURN_SPEED;
            agent.position = vec3_from_ivec3(position);
            agent.position_target = agent.position;
            agent.orientation = quaternion_from_euler_degrees(
                rotation_degrees.x,
                rotation_degrees.y,
                rotation_degrees.z
            );
            agent.rotation_target = rotation_degrees;
            agent.movement_yaw = rotation_degrees.z;
            agent.velocity = vec3_make(0.0f, 0.0f, 0.0f);
            agent.collision_enabled = true;
            agent.collider_radius = vec3_make(0.40f, 0.40f, 0.90f);
            agent.is_grounded = false;

            actor_table_insert(&population->actor, &agent);
            decision_table_insert(&population->decision, agent.actor_id, 0);

            LOG_INFO(
                "Generated %s agent, ID: %i, at (%.1f %.1f %.1f)",
                NATION_TYPE_NAME[nation->nation_type],
                (int)agent.actor_id,
                (double)agent.position.x,
                (double)agent.position.y,
                (double)agent.position.z
            );
        }
    }
}

// Documented in src/app/population.h.
void
population_init(Population* population)
{
    random_seed(&population->random, POPULATION_SEED);

    population->judge_id = TABLE_KEY_NONE;

    population_init_judge(population);
    population_init_agents(population);

    LOG_INFO("POPULATION INIT");
}

// Documented in src/app/population.h.
void
population_quit(Population* population)
{
    actor_table_free(&population->actor);
    decision_table_free(&population->decision);

    population->judge_id = TABLE_KEY_NONE;
}
