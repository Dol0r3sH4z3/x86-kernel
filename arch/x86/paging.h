#ifndef PAGING_H
#define PAGING_H

#include <stdint.h>

#define SLAB_HEAP_BASE 0x40000000u

void vmm_init(void);

#endif