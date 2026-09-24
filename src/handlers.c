#include "handlers.h"
#include "pic.h"
#include "console.h"
#include "vga.h"

void exception_handler()
{
    terminal_error("Critical error.\n");
    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}

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

            if (ascii == '\b')
            {
                terminal_backspace();
            }

            else if (ascii != 0)
            {
                char str[2] = {ascii, '\0'};
                terminal_writestring(str);
            }
        }
    }

    PIC_sendEOI(1);
}
