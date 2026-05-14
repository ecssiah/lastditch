#include "app.h"

#include "core/log.h"

static void 
queue_move_action(const Platform& platform, State& state)
{
    Action move_action = {
        .type = ActionType::Move,
        .action_value = vec3_broadcast(0.0f),
    };

    if (input_button_is_down(platform.input, Button::A))
    {
        move_action.action_value.x -= 1.0f;
    }

    if (input_button_is_down(platform.input, Button::D))
    {
        move_action.action_value.x += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::W))
    {
        move_action.action_value.y += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::S))
    {
        move_action.action_value.y -= 1.0f;
    }

    move_action.action_value = normalize(move_action.action_value);

    if (input_button_is_down(platform.input, Button::E))
    {
        move_action.action_value.z += 1.0f;
    }

    if (input_button_is_down(platform.input, Button::Q))
    {
        move_action.action_value.z -= 1.0f;
    }

    action_add(state.action_queue, move_action);
}

static void 
queue_rotate_action(const Platform& platform, State& state)
{
    const Action rotate_action = {
        .type = ActionType::Rotate,
        .action_value = {
            static_cast<f32>(platform.input.pointer_delta_x),
            static_cast<f32>(platform.input.pointer_delta_y),
            0.0f,
        },
    };

    action_add(state.action_queue, rotate_action);
}

static void 
queue_jump_action(State& state)
{
    const Action jump_action = {
        .type = ActionType::Jump,
        .action_value = vec3_broadcast(1.0f),
    };

    action_add(state.action_queue, jump_action);
}

static void 
queue_debug_mode_action(State& state)
{
    const Action debug_action = {
        .type = ActionType::DebugMode,
        .action_value = vec3_broadcast( 1.0f),
    };

    action_add(state.action_queue, debug_action);
}

void
app_init(State& state, Platform& platform)
{
    log_init();
    
    state.active = true;
    state.evolving = true;

    constexpr b32 random_seed = false;

    state.seed = false ? static_cast<u32>(time(NULL)) : 813;

    srand(state.seed);

    debug_init(state.debug);

    work_init(state.work);
    navigation_init(state.navigation);

    world_init(state.world, state.debug);
    population_init(state.population, state.work);

    render_init(state.render, platform, state.population, state.world);
    screen_init(state.screen, platform);
}

void
app_update(State& state, Platform& platform)
{
    const i32 judge_id = state.population.judge_id;
    Actor& judge = state.population.actor_pool.actor_array[judge_id];

    action_apply_queue(state.action_queue, judge);

    work_update(state.population, state.work, platform.delta_time);
    world_update(state.world, state.population);

    state.world.delta_time = platform.delta_time;

    queue_move_action(platform, state);

    if (fabs(platform.input.pointer_delta_x) > 1e-12f || fabs(platform.input.pointer_delta_y) > 1e-12f)
    {
        queue_rotate_action(platform, state);
    }

    if (input_button_is_pressed(platform.input, Button::Space))
    {
        queue_jump_action(state);
    }

    if (input_button_is_released(platform.input, Button::Tab))
    {
        queue_debug_mode_action(state);
    }
    
    render_update(state.render, state.population, state.debug);
    screen_update(state.screen, state.population);
}

void
app_quit(State& state)
{
    debug_close(state.debug);

    population_quit();
    world_quit(state.world);
    
    log_quit();
}