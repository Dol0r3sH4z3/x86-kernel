#include "pmm.h"

#define BITMAP_SIZE (1024 * 128)
#define PMM_PAGE_SIZE 4096

uint8_t pmm_bitmap[BITMAP_SIZE];

static inline void bit_set(uint8_t *bit_arr, size_t idx)
{
    bit_arr[idx / 8] |= (1 << (idx % 8));
};
static inline void bit_clear(uint8_t *bit_arr, size_t idx)
{
    bit_arr[idx / 8] &= ~(1 << (idx % 8));
};
static inline int bit_test(uint8_t *bit_arr, size_t idx)
{
    return (bit_arr[idx / 8] & (1 << (idx % 8))) != 0;
};
void *memset(void *dest, int ch, size_t count)
{
    uint8_t *ptr = (uint8_t *)dest;
    while (count > 0)
    {
        *ptr = (uint8_t)ch;
        ptr++;
        count--;
    }
    return dest;
}

void pmm_init(void)
{
    memset(pmm_bitmap, 0xFF, BITMAP_SIZE);
};
void pmm_set_region(uint32_t start_addr, uint32_t length, pmm_action_t action)
{
    if (length == 0)
        return;

    uint32_t end_addr = start_addr + length;

    uint32_t page_start = (start_addr) / PMM_PAGE_SIZE;
    uint32_t page_end = (end_addr + PMM_PAGE_SIZE - 1) / PMM_PAGE_SIZE;

    for (uint32_t i = page_start; i < page_end; i++)
    {
        if (action == PMM_REGION_FREE)
        {
            bit_clear(pmm_bitmap, i);
        }
        else
        {
            bit_set(pmm_bitmap, i);
        }
    }
};

void *pmm_alloc_page(void)
{
    for (uint32_t i = 0; i < BITMAP_SIZE * 8; i++)
    {
        if (!bit_test(pmm_bitmap, i))
        {
            bit_set(pmm_bitmap, i);
            return (void *)(i * PMM_PAGE_SIZE);
        }
    }
    return NULL;
};
void pmm_free_page(void *addr)
{
    uint32_t page_index = ((uint32_t)addr) / PMM_PAGE_SIZE;
    bit_clear(pmm_bitmap, page_index);
};