bits 32

MB_ALIGN equ 1 << 0
MB_MEMINFO equ 1 << 1
MB_FLAGS equ MB_ALIGN | MB_MEMINFO
MB_MAGIC equ 0x1BADB002
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

KERNEL_VMA equ 0xC0000000
%define V2P(x) ((x) - KERNEL_VMA)

section .multiboot.data progbits alloc write align=4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .bootstrap_stack nobits alloc write align=16
stack_bottom:
    resb 16384
stack_top:

section .bss nobits alloc write align=4096
boot_page_directory:
    resb 4096
boot_page_table1:
    resb 4096

section .multiboot.text progbits alloc exec nowrite align=16
global _start
extern _kernel_start
extern _kernel_end
extern kernel_main

_start:
    mov edi, V2P(boot_page_table1)
    mov esi, 0
    mov ecx, 1023

.map_loop:
    cmp esi, _kernel_start
    jl .next
    cmp esi, V2P(_kernel_end)
    jge .map_done

    mov edx, esi
    or edx, 0x003
    mov [edi], edx

.next:
    add edi, 4
    add esi, 4096
    loop .map_loop

.map_done:
    mov dword [V2P(boot_page_table1) + 1023*4], 0x000B8000 | 0x003
    mov dword [V2P(boot_page_directory) + 0*4], V2P(boot_page_table1) + 0x003
    mov dword [V2P(boot_page_directory) + 768*4], V2P(boot_page_table1) + 0x003

    mov ecx, V2P(boot_page_directory)
    mov cr3, ecx

    mov ecx, cr0
    or ecx, 0x80010000
    mov cr0, ecx

    mov ecx, higher_half
    jmp ecx

section .text
higher_half:
    mov dword [boot_page_directory + 0], 0

    mov ecx, cr3
    mov cr3, ecx

    mov esp, stack_top

    cmp eax, 0x2BADB002
    jne .hang

    push ebx

    call kernel_main

    cli
.hang:
    hlt
    jmp .hang