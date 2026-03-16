#ifndef PMM_H
#define PMM_H

#include "multiboot.h"

/* Tamanho de um page frame: 4KB */
#define PMM_BLOCK_SIZE  4096

/* Offset para converter enderecos fisicos -> virtuais no higher half */
#define KERNEL_VIRTUAL_BASE 0xC0000000

/* Inicializa o gerenciador de memoria fisica usando o mapa de memoria do GRUB */
void pmm_init(multiboot_info_t* mbinfo, uint32_t kernel_phys_start, uint32_t kernel_phys_end);

/* Aloca um page frame (4KB) e retorna seu endereco FISICO */
void* pmm_alloc_page(void);

/* Libera um page frame dado seu endereco FISICO */
void pmm_free_page(void* page);

#endif
