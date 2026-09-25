#include "vga.h"
#include "console.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

size_t strlen(const char *str, size_t maxlen)
{
    size_t len = 0;
    while (len < maxlen && str[len] != '\0')
    {
        len++;
    }
    return len;
}

bool strcmp(const char *src, const char *cmp)
{
    size_t i = 0;

    while (src[i] == cmp[i])
    {
        if (src[i] == '\0')
        {
            return true;
        }
        i++;
    }

    return false;
}
size_t terminal_input_start_column = 2;

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

void terminal_initialize(void)
{
    terminal_row = 0;
    terminal_column = 0;
    terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);

    for (size_t y = 0; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t index = y * VGA_WIDTH + x;
            terminal_buffer[index] = vga_entry(' ', terminal_color);
        }
    }
}

void terminal_setcolor(uint8_t color)
{
    terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c)
{
    if (c == '\n')
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
        {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }
    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
        {
            terminal_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void terminal_write(const char *data, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        terminal_putchar(data[i]);
    }
}

void terminal_writestring(const char *data)
{
    terminal_write(data, strlen(data, 4096));
}

void terminal_error(const char *data)
{
    terminal_setcolor(VGA_COLOR_RED);
    terminal_writestring(data);
    terminal_setcolor(VGA_COLOR_WHITE);
}

void terminal_warn(const char *data)
{
    terminal_setcolor(VGA_COLOR_BROWN);
    terminal_writestring(data);
    terminal_setcolor(VGA_COLOR_WHITE);
}

void terminal_scroll()
{
    for (size_t y = 1; y < VGA_HEIGHT; y++)
    {
        for (size_t x = 0; x < VGA_WIDTH; x++)
        {
            const size_t src_index = y * VGA_WIDTH + x;
            const size_t dst_index = (y - 1) * VGA_WIDTH + x;
            terminal_buffer[dst_index] = terminal_buffer[src_index];
        }
    }

    const size_t last_row = VGA_HEIGHT - 1;
    for (size_t x = 0; x < VGA_WIDTH; x++)
    {
        const size_t index = last_row * VGA_WIDTH + x;
        terminal_buffer[index] = vga_entry(' ', terminal_color);
    }
}

void terminal_backspace(void)
{
    if (is_interactive && terminal_column <= terminal_input_start_column)
    {
        return;
    }

    if (terminal_column == 0)
    {
        if (terminal_row > 0)
        {
            terminal_row--;
            terminal_column = VGA_WIDTH - 1;
        }
        else
        {
            return;
        }
    }
    else
    {
        terminal_column--;
    }

    terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
}

void execute_command(const char *cmd)
{
    terminal_putchar('\n');

    if (strcmp(cmd, "help"))
    {
        terminal_writestring("Available commands: help, clear\n");
    }
    else if (strcmp(cmd, "clear"))
    {
        terminal_initialize();
    }
    else if (cmd[0] != '\0')
    {
        terminal_writestring("Unknown command: ");
        terminal_writestring(cmd);
        terminal_putchar('\n');
    }

    terminal_writestring("> ");
    terminal_input_start_column = terminal_column;
}