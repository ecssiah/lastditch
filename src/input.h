#ifndef INPUT_H
#define INPUT_H

#include "ld_data.h"

b32 input_key_is_down(Shell *shell, int key);
b32 input_key_is_pressed(Shell *shell, int key);
b32 input_key_is_released(Shell *shell, int key);

b32 input_button_is_down(Shell *shell, int button);
b32 input_button_is_pressed(Shell *shell, int button);
b32 input_button_is_released(Shell *shell, int button);

void input_init(Shell *shell);
void input_update(Shell *shell, Sim *sim);

#endif
