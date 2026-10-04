#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

typedef struct
{
    uint32_t edi, esi, ebp, esp, ebx, edx, ecx, eax;
    uint32_t int_no, error_code;
    uint32_t eip, cs, eflags;
} __attribute__((packed)) registers_t;

__attribute__((noreturn)) void exception_handler(registers_t *regs);

#endif