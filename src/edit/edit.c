#include <string.h>

#include "core/action.h"
#include "core/core_data.h"
#include "edit/edit_data.h"
#include "platform/platform_data.h"
#include "platform/platform.h"
#include "shell/shell_data.h"
#include "shell/shell.h"
#include "shell/render.h"
#include "shell/screen.h"

static Platform platform;
static Shell shell;
static Edit edit;

static void update_time(Edit *edit)
{
    edit->current_time = glfwGetTime();

    edit->delta_time = (edit->previous_time > 0.0)
        ? (f32)(edit->current_time - edit->previous_time)
        : 0.0f;

    edit->previous_time = edit->current_time;
}

static void apply_action(Edit *edit, Action *action)
{
    switch (action->type)
    {
    default: break;
    }
}

static void apply_action_queue(Edit *edit)
{
    while (edit->action_queue.head_index != edit->action_queue.tail_index)
    {
        Action action;
        action_pop(&edit->action_queue, &action);

        apply_action(edit, &action);
    }
}

void edit_init(Edit *edit)
{
    edit->active = TRUE;
    
    edit->current_time = 0.0;
    edit->previous_time = 0.0;

    edit->delta_time = 0.0f;

    edit->cell_array = calloc(EDIT_VOLUME_IN_CELLS, sizeof(Cell));

    i32 cell_index;
    for (cell_index = 0; cell_index < EDIT_VOLUME_IN_CELLS; ++cell_index)
    {
        edit->cell_array[cell_index].cell_index = cell_index;
    }
}

void edit_update(Edit *edit)
{
    update_time(edit);

    apply_action_queue(edit);
}

void edit_close(Edit *edit)
{
    free(edit->cell_array);
}

int main()
{
    platform_init(&platform);
    shell_init(&shell);
    edit_init(&edit);
    
    render_init(&shell, &edit);
    screen_init(&shell);
    
    while (shell.active)
    {
        platform_update(&platform);
        shell_update(&shell);

        edit_update(&edit);

        render_update(&shell, &edit);
        screen_update(&shell, &edit);
    }

    edit_close(&edit);
    shell_close(&shell);
    platform_close(&platform);
}
