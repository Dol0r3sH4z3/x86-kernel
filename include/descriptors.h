#ifndef DESCRIPTORS_H
#define DESCRIPTORS_H

#include <stdint.h>

typedef enum function_status
{
    FUNCTION_STATUS_SUCCESS,
    FUNCTION_STATUS_ERROR,
} function_status_t;

struct GDT
{
    uint32_t base;
    uint32_t limit;
    uint8_t access_byte;
    uint8_t flags;
};

struct gdt_pointer
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

typedef struct
{
    uint16_t isr_low;
    uint16_t kernel_cs;
    uint8_t reserved;
    uint8_t attributes;
    uint16_t isr_high;
} __attribute__((packed)) idt_entry_t;

typedef struct
{
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idtr_t;

function_status_t encodeGdtEntry(uint8_t *target, struct GDT source);
function_status_t initialize_gdt(void);

void initialize_idt(void);
void idt_set_descriptor(uint8_t vector, void *isr, uint8_t flags);

#endif