#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>

extern bool is_interactive;

void console_enable_input(void);
void console_disable_input(void);

#endif