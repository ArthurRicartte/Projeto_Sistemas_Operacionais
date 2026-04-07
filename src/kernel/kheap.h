#ifndef KHEAP_H
#define KHEAP_H

#include "multiboot.h"

/* Aloca memoria dinamica no kernel (retorna endereco FISICO do page frame) */
void* kmalloc(uint32_t size);

/* Libera memoria alocada com kmalloc */
void kfree(void* ptr);

#endif
