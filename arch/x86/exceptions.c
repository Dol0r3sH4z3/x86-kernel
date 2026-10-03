#include <kernel/console.h>
#include "exceptions.h"

void exception_handler()
{
    t_error("Critical error.\n");
    while (1)
    {
        __asm__ volatile("cli; hlt");
    }
}
