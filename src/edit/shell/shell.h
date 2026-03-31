#ifndef EDIT_SHELL_H
#define EDIT_SHELL_H

#include "edit/edit_data.h"
#include "shell/shell_data.h"

void edit_shell_init(Shell *shell);
void edit_shell_update(Shell *shell, Edit *edit);
void edit_shell_close(Shell *shell);

#endif
