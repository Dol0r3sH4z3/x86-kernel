#include "console.h"

bool is_interactive = false;

void console_enable_input() { is_interactive = true; }
void console_disable_input() { is_interactive = false; }