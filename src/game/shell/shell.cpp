#include "game/shell/shell.h"

#include "core/log.h"
#include "game/sim/action.h"
#include "game/shell/render.h"
#include "game/shell/screen.h"
#include "platform/platform.h"

static void queue_move_action(Platform* platform, Sim* sim)
{
    Action move_action = {
        .type = ActionType::move,
        .action_value = {0.0f, 0.0f, 0.0f},
    };

    if (platform_button_is_down(platform, Button::a))
    {
        move_action.action_value[0] -= 1.0f;
    }

    if (platform_button_is_down(platform, Button::d))
    {
        move_action.action_value[0] += 1.0f;
    }

    if (platform_button_is_down(platform, Button::w))
    {
        move_action.action_value[1] += 1.0f;
    }

    if (platform_button_is_down(platform, Button::s))
    {
        move_action.action_value[1] -= 1.0f;
    }

    glm_vec3_normalize(move_action.action_value);

    if (platform_button_is_down(platform, Button::e))
    {
        move_action.action_value[2] += 1.0f;
    }

    if (platform_button_is_down(platform, Button::q))
    {
        move_action.action_value[2] -= 1.0f;
    }

    action_add(&sim->action_queue, &move_action);
}

static void queue_rotate_action(Platform* platform, Sim* sim)
{
    Action rotate_action = {
        .type = ActionType::rotate,
        .action_value = {
            static_cast<f32>(platform->input.pointer_delta_x),
            static_cast<f32>(platform->input.pointer_delta_y),
            0.0f,
        },
    };

    action_add(&sim->action_queue, &rotate_action);
}

static void queue_jump_action(Sim* sim)
{
    Action jump_action = {
        .type = ActionType::jump,
        .action_value = {1.0f, 0.0f, 0.0f},
    };

    action_add(&sim->action_queue, &jump_action);
}

static void queue_debug_mode_action(Sim* sim)
{
    Action debug_action = {
        .type = ActionType::debug_mode,
        .action_value = GLM_VEC3_ONE_INIT,
    };

    action_add(&sim->action_queue, &debug_action);
}

void shell_init(Shell* shell)
{
    log_init();

    shell->active = true;
}

void shell_update(Platform* platform, Sim* sim)
{
    sim->world.delta_time = platform->delta_time;

    queue_move_action(platform, sim);

    if (fabs(platform->input.pointer_delta_x) > 1e-12f || fabs(platform->input.pointer_delta_y) > 1e-12f)
    {
        queue_rotate_action(platform, sim);
    }

    if (platform_button_is_pressed(platform, Button::space))
    {
        queue_jump_action(sim);
    }

    if (platform_button_is_released(platform, Button::tab))
    {
        queue_debug_mode_action(sim);
    }
}

void shell_present(Shell* shell, Sim* sim)
{
    render_update(shell, sim);
    screen_update(shell, sim);
}

void shell_close()
{
    log_close();
}
