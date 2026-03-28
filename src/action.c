#include "action.h"
#include "ld_data.h"

void action_add(ActionQueue *action_queue, Action action)
{
    action_queue->action_array[action_queue->tail_index++] = action;
}
