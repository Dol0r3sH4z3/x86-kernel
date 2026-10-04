#ifndef CONSOLE_H
#define CONSOLE_H

#include <stdbool.h>
#include <stdint.h>

void console_enable_input(void);
void console_disable_input(void);
bool console_is_interactive(void);

void t_print(const char *data);
void t_error(const char *data);
void t_success(const char *data);
void t_warn(const char *data);
void t_hex(uintptr_t data);

void execute_command(const char *cmd);

#endif