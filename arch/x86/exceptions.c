#include "exceptions.h"

void exception_handler()
{
    terminal_error("Critical error.\n");
    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}
