#ifndef ACTION_H
#define ACTION_H

#include "ld_data.h"

void action_add(ActionQueue *action_queue, Action action);
b32 action_pop(ActionQueue *action_queue, Action *out_action);

#endif
