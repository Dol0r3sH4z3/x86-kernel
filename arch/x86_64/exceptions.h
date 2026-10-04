// exceptions.h
#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H
#include <stdint.h>

typedef struct
{
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, error_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed)) registers_t;

__attribute__((noreturn)) void exception_handler(registers_t *regs);
#endif