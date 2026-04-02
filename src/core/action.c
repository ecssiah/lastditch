#include "jsk_log.h"

#include "core/action.h"
#include "core/core_data.h"

void action_add(ActionQueue *action_queue, Action action)
{
    i32 tail_index_next = (action_queue->tail_index + 1) % ACTION_QUEUE_CAPACITY;

    if (tail_index_next == action_queue->head_index)
    {
        LOG_WARN("ActionQueue is full");
        return;
    }
    
    action_queue->action_array[action_queue->tail_index] = action;

    action_queue->tail_index = tail_index_next;
}

bool action_pop(ActionQueue *action_queue, Action *out_action)
{
    if (action_queue->tail_index == action_queue->head_index)
    {
        return false;
    }

    *out_action = action_queue->action_array[action_queue->head_index];

    action_queue->head_index = (action_queue->head_index + 1) % ACTION_QUEUE_CAPACITY;

    return true;
}
