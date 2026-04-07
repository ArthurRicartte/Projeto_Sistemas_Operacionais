#include "kheap.h"
#include "pmm.h"

/*
 * Implementacao basica do heap do kernel.
 * Por enquanto, cada chamada a kmalloc aloca um page frame inteiro (4KB).
 * Uma implementacao mais avancada dividiria os page frames em blocos menores.
 */

void *kmalloc(uint32_t size)
{
    if (size == 0)
        return 0;

    /* Aloca um page frame via PMM */
    void *page = pmm_alloc_page();
    return page;
}

void kfree(void *ptr)
{
    if (ptr)
    {
        pmm_free_page(ptr);
    }
}
