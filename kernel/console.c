#include <kernel/console.h>
#include <kernel/string.h>
#include <arch/arch.h>
#include "vga.h"

bool is_interactive = false;

void console_enable_input() { is_interactive = true; }
void console_disable_input() { is_interactive = false; }
bool console_is_interactive() { return is_interactive; }

void t_print(const char *data)
{
    for (size_t i = 0; i < strlen(data, 4096); i++)
    {
        arch_colsole_putc(data[i]);
    }
}

void t_error(const char *data)
{
    arch_console_set_color(COLOR_RED);
    t_print(data);
    arch_console_set_color(COLOR_DEFAULT);
}

void t_warn(const char *data)
{
    arch_console_set_color(COLOR_YELLOW);
    t_print(data);
    arch_console_set_color(COLOR_DEFAULT);
}

void t_success(const char *data)
{
    arch_console_set_color(COLOR_GREEN);
    t_print(data);
    arch_console_set_color(COLOR_DEFAULT);
}

void t_hex(uint32_t data)
{
    static const char hex_digits[] = "0123456789ABCDEF";

    t_print("0x");

    for (int i = 28; i >= 0; i -= 4)
    {
        uint8_t nibble = (data >> i) & 0xF;
        arch_colsole_putc(hex_digits[nibble]);
    }
}

void execute_command(const char *cmd)
{
    arch_colsole_putc('\n');

    if (strcmp(cmd, "help"))
    {
        t_print("Available commands: help, clear\n");
    }
    else if (strcmp(cmd, "clear"))
    {
        arch_console_init();
    }
    else if (cmd[0] != '\0')
    {
        t_print("Unknown command: ");
        t_print(cmd);
        arch_colsole_putc('\n');
    }

    t_print("> ");
    arch_console_mark_input_start();
}
