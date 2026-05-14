#include "app/shell.h"

#include "core/log.h"
#include "app/action.h"
#include "app/render.h"
#include "app/screen.h"
#include "platform/platform.h"

static void 
queue_move_action(const Platform& platform, Sim& sim)
{
    Action move_action = {
        .type = ActionType::move,
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

    action_add(sim.action_queue, move_action);
}

static void 
queue_rotate_action(const Platform& platform, Sim& sim)
{
    const Action rotate_action = {
        .type = ActionType::rotate,
        .action_value = {
            static_cast<f32>(platform.input.pointer_delta_x),
            static_cast<f32>(platform.input.pointer_delta_y),
            0.0f,
        },
    };

    action_add(sim.action_queue, rotate_action);
}

static void 
queue_jump_action(Sim& sim)
{
    const Action jump_action = {
        .type = ActionType::jump,
        .action_value = vec3_broadcast(1.0f),
    };

    action_add(sim.action_queue, jump_action);
}

static void 
queue_debug_mode_action(Sim& sim)
{
    const Action debug_action = {
        .type = ActionType::debug_mode,
        .action_value = vec3_broadcast( 1.0f),
    };

    action_add(sim.action_queue, debug_action);
}

void 
shell_init(Shell& shell)
{
    log_init();

    shell.active = true;
}

void 
shell_update(const Platform& platform, Sim& sim)
{
    sim.world.delta_time = platform.delta_time;

    queue_move_action(platform, sim);

    if (fabs(platform.input.pointer_delta_x) > 1e-12f || fabs(platform.input.pointer_delta_y) > 1e-12f)
    {
        queue_rotate_action(platform, sim);
    }

    if (input_button_is_pressed(platform.input, Button::Space))
    {
        queue_jump_action(sim);
    }

    if (input_button_is_released(platform.input, Button::Tab))
    {
        queue_debug_mode_action(sim);
    }
}

void 
shell_present(Shell& shell, Sim& sim)
{
    render_update(shell, sim);
    screen_update(shell, sim);
}

void 
shell_close()
{
    log_close();
}
