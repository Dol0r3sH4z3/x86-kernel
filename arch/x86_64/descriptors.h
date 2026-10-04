// arch/x86_64/descriptors.h
#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H
#include <stdint.h>

typedef enum
{
    FUNCTION_STATUS_SUCCESS,
    FUNCTION_STATUS_ERROR
} function_status_t;

typedef struct
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) dt_ptr_t;

typedef struct
{
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t attributes;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed)) idt_entry_t;

function_status_t init_gdt(void);
void init_idt(void);
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);
#endif