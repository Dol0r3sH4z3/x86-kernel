#ifndef INTERRUPTS_H
#define INTERRUPTS_H

#include <stdint.h>

typedef enum function_status {
    FUNCTION_STATUS_SUCCESS,
    FUNCTION_STATUS_ERROR,
} function_status_t;

struct GDT {
    uint32_t base;
    uint32_t limit;
    uint8_t access_byte;
    uint8_t flags;
};

struct gdt_pointer {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

function_status_t encodeGdtEntry(uint8_t *target, struct GDT source);
function_status_t initialize_gdt(void);

#endif