#ifndef SHELL_H
#define SHELL_H

#include "game/sim/sim_data.h"
#include "game/shell/shell_data.h"
#include "platform/platform_data.h"

void shell_init(Shell *shell);
void shell_update(Shell *shell, Platform *platform, Sim *sim);
void shell_present(Shell *shell, Sim *sim);
void shell_close(Shell *shell);

#endif
