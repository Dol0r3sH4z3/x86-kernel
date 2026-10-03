#ifndef VGA_H
#define VGA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

void t_putentryat(char c, uint8_t color, size_t, size_t y);
void t_scroll(void);
void t_backspace(void);

#endif