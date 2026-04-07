#include "pmm.h"
#include "fb.h"

/* cada bit no bitmap representa um page frame de 4KB */
#define BLOCKS_PER_BYTE 8

/* suporta até 4GB de RAM: 4GB / 4KB = 1048576 page frames */
#define MAX_BLOCKS 1048576

/* bitmap: 1 = usado, 0 = livre */
static uint8_t memory_map[MAX_BLOCKS / BLOCKS_PER_BYTE];

/* contadores */
static uint32_t total_free_blocks = 0;
static uint32_t max_memory_blocks = 0;

/* operacoes no bitmap*/

static inline void bitmap_set(uint32_t bit) {
    memory_map[bit / BLOCKS_PER_BYTE] |= (1 << (bit % BLOCKS_PER_BYTE));
}

static inline void bitmap_unset(uint32_t bit) {
    memory_map[bit / BLOCKS_PER_BYTE] &= ~(1 << (bit % BLOCKS_PER_BYTE));
}

static inline int bitmap_test(uint32_t bit) {
    return memory_map[bit / BLOCKS_PER_BYTE] & (1 << (bit % BLOCKS_PER_BYTE));
}


/* funcoes internas */
static void pmm_init_region(uint32_t base, uint32_t size) {
    uint32_t align = base / PMM_BLOCK_SIZE;
    uint32_t blocks = size / PMM_BLOCK_SIZE;
    for (uint32_t i = 0; i < blocks; i++) {
        bitmap_unset(align + i);
        total_free_blocks++;
    }
}

/* marca uma regiao de memoria como USADA (reservada) */
static void pmm_deinit_region(uint32_t base, uint32_t size) {
    uint32_t align = base / PMM_BLOCK_SIZE;
    uint32_t blocks = (size + PMM_BLOCK_SIZE - 1) / PMM_BLOCK_SIZE; /* arredonda pra cima */
    for (uint32_t i = 0; i < blocks; i++) {
        bitmap_set(align + i);
        if (total_free_blocks > 0) total_free_blocks--;
    }
}

/* funcoes gerais */

void pmm_init(multiboot_info_t* mbinfo, uint32_t kernel_phys_start, uint32_t kernel_phys_end) {
    /* Inicialmente marca TODA memoria como usada (bits = 1) */
    for (uint32_t i = 0; i < MAX_BLOCKS / BLOCKS_PER_BYTE; i++) {
        memory_map[i] = 0xFF;
    }

    total_free_blocks = 0;
    max_memory_blocks = 0;

    /* verifica se o GRUB forneceu o mapa de memoria (flag bit 6) */
    if (!(mbinfo->flags & MULTIBOOT_INFO_MEM_MAP)) {
        fb_write("ERRO: GRUB nao forneceu mapa de memoria!\n", 42);
        return;
    }

    /*
     * IMPORTANTE: Os enderecos no multiboot sao FISICOS.
     * Como estamos no higher half, precisamos somar 0xC0000000
     * para acessar essas estruturas via enderecos virtuais.
     */
    multiboot_memory_map_t* mmap = (multiboot_memory_map_t*)
        (mbinfo->mmap_addr + KERNEL_VIRTUAL_BASE);
    uint32_t mmap_end = mbinfo->mmap_addr + mbinfo->mmap_length + KERNEL_VIRTUAL_BASE;

    /* percorre o mapa de memoria do GRUB */
    while ((uint32_t) mmap < mmap_end) {
        if (mmap->type == MULTIBOOT_MEMORY_AVAILABLE) {
            uint32_t addr = (uint32_t) mmap->addr;
            uint32_t len = (uint32_t) mmap->len;

            /* marca essa regiao como livre no bitmap */
            pmm_init_region(addr, len);

            /* atualiza o numero maximo de blocos */
            uint32_t max_block = (addr + len) / PMM_BLOCK_SIZE;
            if (max_block > max_memory_blocks) {
                max_memory_blocks = max_block;
            }
        }
        /* avanca para a proxima entrada (size + sizeof(size)) */
        mmap = (multiboot_memory_map_t*)
            ((uint32_t) mmap + mmap->size + sizeof(mmap->size));
    }

    /* reserva o primeiro 1MB (BIOS, GRUB, I/O mapeado, etc) */
    pmm_deinit_region(0x0, 0x100000);

    /* reserva a memoria usada pelo kernel */
    pmm_deinit_region(kernel_phys_start, kernel_phys_end - kernel_phys_start);
}

void* pmm_alloc_page(void) {
    /* procura o primeiro bit livre no bitmap */
    for (uint32_t i = 0; i < max_memory_blocks; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            total_free_blocks--;
            return (void*)(i * PMM_BLOCK_SIZE);
        }
    }
    return 0; /* Sem memoria disponivel */
}

void pmm_free_page(void* page) {
    uint32_t addr = (uint32_t) page;
    uint32_t frame = addr / PMM_BLOCK_SIZE;

    if (bitmap_test(frame)) {
        bitmap_unset(frame);
        total_free_blocks++;
    }
}
