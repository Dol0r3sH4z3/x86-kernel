#include <arch/arch.h>
#include "vga.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xC00B8000

typedef enum
{
    VGA_COLOR_BLACK = 0,
    VGA_COLOR_BLUE = 1,
    VGA_COLOR_GREEN = 2,
    VGA_COLOR_CYAN = 3,
    VGA_COLOR_RED = 4,
    VGA_COLOR_MAGENTA = 5,
    VGA_COLOR_BROWN = 6,
    VGA_COLOR_LIGHT_GREY = 7,
    VGA_COLOR_DARK_GREY = 8,
    VGA_COLOR_LIGHT_BLUE = 9,
    VGA_COLOR_LIGHT_GREEN = 10,
    VGA_COLOR_LIGHT_CYAN = 11,
    VGA_COLOR_LIGHT_RED = 12,
    VGA_COLOR_LIGHT_MAGENTA = 13,
    VGA_COLOR_LIGHT_BROWN = 14,
    VGA_COLOR_WHITE = 15,
} vga_color_t;

static inline uint8_t vga_entry_color(vga_color_t fg, vga_color_t bg)
{
    return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color)
{
    return (uint16_t)uc | (uint16_t)color << 8;
}

static size_t terminal_input_start_column = 2;
static size_t terminal_column;

static uint8_t terminal_color;
static size_t terminal_row;
static uint16_t *terminal_buffer = (uint16_t *)VGA_MEMORY;

static void t_putentryat(char c, uint8_t color, size_t x, size_t y)
{
    const size_t index = y * VGA_WIDTH + x;
    terminal_buffer[index] = vga_entry(c, color);
}

static void t_scroll()
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

void arch_console_init()
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

void arch_console_putc(char c)
{
    if (c == '\n')
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
        {
            t_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
        return;
    }
    t_putentryat(c, terminal_color, terminal_column, terminal_row);
    if (++terminal_column == VGA_WIDTH)
    {
        terminal_column = 0;
        if (++terminal_row == VGA_HEIGHT)
        {
            t_scroll();
            terminal_row = VGA_HEIGHT - 1;
        }
    }
}

void arch_console_set_color(arch_color_t c)
{
    switch (c)
    {
    case COLOR_DEFAULT:
        terminal_color = VGA_COLOR_LIGHT_GREY;
        break;

    case COLOR_GREEN:
        terminal_color = VGA_COLOR_GREEN;
        break;

    case COLOR_RED:
        terminal_color = VGA_COLOR_RED;
        break;

    case COLOR_YELLOW:
        terminal_color = VGA_COLOR_BROWN;
        break;

    default:
        break;
    }
};

void t_backspace(void)
{
    if (terminal_column <= terminal_input_start_column)
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

    t_putentryat(' ', terminal_color, terminal_column, terminal_row);
}

void arch_console_mark_input_start(void) { terminal_input_start_column = terminal_column; };