#include "game/shell/shell.h"

#include "game/sim/action.h"
#include "justsky_log.h"

#include "game/shell/render.h"
#include "game/shell/screen.h"
#include "platform/platform.h"

static void queue_move_action(Platform *platform, Sim *sim)
{
    Action move_action = {
        .type = ACTION_MOVE,
        .action_value = { 0.0f, 0.0f, 0.0f },
    };
    
    if (platform_button_is_down(platform, BUTTON_A))
    {
        move_action.action_value[0] -= 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_D))
    {
        move_action.action_value[0] += 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_W))
    {
        move_action.action_value[1] += 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_S))
    {
        move_action.action_value[1] -= 1.0f;
    }

    move_action.action_value = glm::normalize(move_action.action_value);
    
    if (platform_button_is_down(platform, BUTTON_E))
    {
        move_action.action_value[2] += 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_Q))
    {
        move_action.action_value[2] -= 1.0f;
    }
        
    action_add(&sim->action_queue, &move_action);        
}

static void queue_rotate_action(Platform *platform, Sim *sim)
{
    Action rotate_action = {
        .type = ACTION_ROTATE,
        .action_value = {
            (f32)platform->input.pointer_delta_x,
            (f32)platform->input.pointer_delta_y,
            0.0f,
        },
    };
    
    action_add(&sim->action_queue, &rotate_action);
}

static void queue_jump_action(Sim *sim)
{
    Action jump_action = {
        .type = ACTION_JUMP,
        .action_value = { 1.0f, 0.0f, 0.0f },
    };

    action_add(&sim->action_queue, &jump_action);
}

static void queue_debug_mode_action(Sim *sim)
{
    Action debug_action = {
        .type = ACTION_DEBUG_MODE,
        .action_value = vec3(1, 1, 1),
    };

    action_add(&sim->action_queue, &debug_action);
}

void shell_init(Shell *shell)
{
    log_init();
    
    shell->active = true;
}

void shell_update(Platform *platform, Sim *sim)
{
    sim->world.delta_time = platform->delta_time;

    queue_move_action(platform, sim);
    
    if (fabs(platform->input.pointer_delta_x) > 1e-12f || fabs(platform->input.pointer_delta_y) > 1e-12f)
    {
        queue_rotate_action(platform, sim);
    }

    if (platform_button_is_pressed(platform, BUTTON_SPACE))
    {
        queue_jump_action(sim);
    }

    if (platform_button_is_released(platform, BUTTON_TAB))
    {
        queue_debug_mode_action(sim);
    }
}

void shell_present(Shell *shell, Sim *sim)
{
    render_update(shell, sim);
    screen_update(shell, sim);
}

void shell_close()
{
    log_close();
}

