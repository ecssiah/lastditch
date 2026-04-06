#ifndef ACTION_H
#define ACTION_H

#include "core/core.h"

void action_add(ActionQueue *action_queue, Action action);
bool action_pop(ActionQueue *action_queue, Action *out_action);

#endif
