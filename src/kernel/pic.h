#ifndef PIC_H
#define PIC_H
#include <stdint.h>

void init_pic(void);
void pic_acknowledge(unsigned int interrupt);
void pic_clear_mask(uint8_t irq);

#endif