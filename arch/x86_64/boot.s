bits 32

MB_FLAGS    equ (1 << 0) | (1 << 1)
MB_MAGIC    equ 0x1BADB002
MB_CHECKSUM equ -(MB_MAGIC + MB_FLAGS)

section .multiboot.data progbits alloc noexec nowrite align=4
    dd MB_MAGIC
    dd MB_FLAGS
    dd MB_CHECKSUM

section .boot.bss nobits alloc write align=4096
boot_pml4: resb 4096
boot_pdpt: resb 4096
boot_pd:   resb 4096


section .multiboot.text progbits alloc exec nowrite align=16
global _start
extern arch_main

_start:
    cli
    cmp eax, 0x2BADB002          
    jne .hang
    mov esi, eax                 

    mov edi, boot_pml4
    mov ecx, 3 * 1024            
    xor eax, eax
    rep stosd

    mov eax, boot_pdpt
    or  eax, 0x3                 
    mov [boot_pml4 + 0   * 8], eax
    mov [boot_pml4 + 256 * 8], eax
    mov [boot_pml4 + 511 * 8], eax

    mov eax, boot_pd
    or  eax, 0x3
    mov [boot_pdpt + 0   * 8], eax   
    mov [boot_pdpt + 510 * 8], eax   

    xor ecx, ecx
.fill_pd:
    mov eax, ecx
    shl eax, 21                  
    or  eax, 0x83                
    mov [boot_pd + ecx * 8], eax
    mov dword [boot_pd + ecx * 8 + 4], 0
    inc ecx
    cmp ecx, 512
    jne .fill_pd

    mov eax, cr4
    or  eax, 1 << 5              
    mov cr4, eax

    mov eax, boot_pml4
    mov cr3, eax                 

    mov ecx, 0xC0000080          
    rdmsr
    or  eax, 1 << 8              
    wrmsr

    mov eax, cr0
    or  eax, 1 << 31             
    mov cr0, eax

    lgdt [gdt64.pointer]
    jmp 0x08:long_mode_start     

.hang:
    hlt
    jmp .hang

align 8
gdt64:
    dq 0                         
    dq 0x00AF9A000000FFFF        
    dq 0x00CF92000000FFFF        
.pointer:
    dw $ - gdt64 - 1
    dd gdt64

bits 64
long_mode_start:
    mov edi, ebx                 
    mov esi, esi                 
    mov rax, higher_half         
    jmp rax

section .text
higher_half:
    xor eax, eax
    mov ss, ax                   
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    mov rsp, stack_top
    xor ebp, ebp
    call arch_main               

    cli
.hang:
    hlt
    jmp .hang

section .bss nobits alloc write align=16
    stack_bottom: resb 16384
    stack_top: