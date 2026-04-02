#include "game/shell/shell.h"

#include "jsk_log.h"

#include "core/action.h"
#include "core/core_data.h"
#include "game/sim/sim_data.h"
#include "game/shell/render.h"
#include "game/shell/screen.h"
#include "game/shell/shell_data.h"
#include "platform/platform.h"
#include "platform/platform_data.h"

static void queue_move_action(Platform *platform, Sim *sim)
{
    vec3 action_value;

    if (platform_button_is_down(platform, BUTTON_A))
    {
        action_value[0] -= 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_D))
    {
        action_value[0] += 1.0f;
    }

    if (platform_button_is_down(platform, BUTTON_W))
    {
        action_value[1] += 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_S))
    {
        action_value[1] -= 1.0f;
    }
    
    glm_vec3_normalize(action_value);

    if (platform_button_is_down(platform, BUTTON_E))
    {
        action_value[2] += 1.0f;
    }
    
    if (platform_button_is_down(platform, BUTTON_Q))
    {
        action_value[2] -= 1.0f;
    }
    
    Action move_action;
    move_action.type = ACTION_MOVE;

    glm_vec3_copy(action_value, move_action.action_value);
        
    action_add(&sim->action_queue, move_action);        
}

static void queue_rotate_action(Platform *platform, Sim *sim)
{
    Action rotate_action;
    rotate_action.type = ACTION_ROTATE;
    rotate_action.action_value[0] = (f32)platform->input.pointer_delta_x;
    rotate_action.action_value[1] = (f32)platform->input.pointer_delta_y;
    rotate_action.action_value[2] = 0.0f;

    action_add(&sim->action_queue, rotate_action);
}

static void queue_jump_action(Platform *platform, Sim *sim)
{
    Action jump_action;
    jump_action.type = ACTION_JUMP;
    jump_action.action_value[0] = 1.0f;

    action_add(&sim->action_queue, jump_action);
}

static void queue_debug_mode_action(Platform *platform, Sim *sim)
{
    Action debug_action;
    debug_action.type = ACTION_DEBUG_MODE;
    debug_action.action_value[0] = 1.0f;

    action_add(&sim->action_queue, debug_action);
}

void shell_init(Shell *shell)
{
    log_init();
    
    shell->active = true;
}

void shell_update(Shell *shell, Platform *platform, Sim *sim)
{
    sim->time.delta_time = platform->delta_time;

    queue_move_action(platform, sim);
    
    if (fabs(platform->input.pointer_delta_x) > 1e-12f || fabs(platform->input.pointer_delta_y) > 1e-12f)
    {
        queue_rotate_action(platform, sim);
    }

    if (platform_button_is_pressed(platform, BUTTON_SPACE))
    {
        queue_jump_action(platform, sim);
    }

    if (platform_button_is_released(platform, BUTTON_TAB))
    {
        queue_debug_mode_action(platform, sim);
    }
}

void shell_present(Shell *shell, Sim *sim)
{
    render_update(shell, sim);
    screen_update(shell, sim);
}

void shell_close(Shell *shell)
{
    log_close();
}

