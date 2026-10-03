#include "keyboard.h"
#include "pic.h"
#include <kernel/console.h>
#include "vga.h"

#define INPUT_BUFFER_SIZE 256

char input_buffer[INPUT_BUFFER_SIZE];
size_t input_buffer_idx = 0;

const char kbdus[128] = {
    0,
    27,
    '1',
    '2',
    '3',
    '4',
    '5',
    '6',
    '7',
    '8',
    '9',
    '0',
    '-',
    '=',
    '\b',
    '\t', /* Табуляция */
    'q',
    'w',
    'e',
    'r',
    't',
    'y',
    'u',
    'i',
    'o',
    'p',
    '[',
    ']',
    '\n', /* Enter */
    0,    /* 29   - Control */
    'a',
    's',
    'd',
    'f',
    'g',
    'h',
    'j',
    'k',
    'l',
    ';',
    '\'',
    '`',
    0, /* Левый Shift */
    '\\',
    'z',
    'x',
    'c',
    'v',
    'b',
    'n',
    'm',
    ',',
    '.',
    '/',
    0, /* Правый Shift */
    '*',
    0,   /* Alt */
    ' ', /* Пробел */
    0,   /* Caps Lock */
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0,
    0, /* Клавиши F1-F10 */
    0, /* Num Lock */
    0, /* Scroll Lock */
    0, /* Home */
    0, /* Стрелка вверх */
    0, /* Page Up */
    '-',
    0, /* Стрелка влево */
    0,
    0, /* Стрелка вправо */
    '+',
    0, /* End */
    0, /* Стрелка вниз */
    0, /* Page Down */
    0, /* Insert */
    0, /* Delete */
    0,
    0,
    0,
    0, /* F11 */
    0, /* F12 */
    0, /* Все остальные клавиши не определены */
};

void keyboard_handler_main()
{
    uint8_t scancode = inb(0x60);

    if (!is_interactive)
    {
        PIC_sendEOI(1);
        return;
    }

    if (scancode & 0x80)
    {
        // RELEASED
    }
    else
    {
        // PRESSED
        if (scancode < 128)
        {
            char ascii = kbdus[scancode];

            if (ascii == '\n')
            {
                input_buffer[input_buffer_idx] = '\0';
                execute_command(input_buffer);
                input_buffer_idx = 0;
            }

            else if (ascii == '\b')
            {
                extern size_t terminal_column;
                extern size_t terminal_input_start_column;

                if (terminal_column > terminal_input_start_column && input_buffer_idx > 0)
                {
                    input_buffer_idx--;
                    terminal_backspace();
                }
            }

            else if (ascii != 0)
            {
                input_buffer[input_buffer_idx++] = ascii;

                char str[2] = {ascii, '\0'};
                terminal_print(str);
            }
        }
    }

    PIC_sendEOI(1);
}
