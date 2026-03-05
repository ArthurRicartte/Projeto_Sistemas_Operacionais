#ifndef INCLUDE_IO_H
#define INCLUDE_IO_H

/** outb – envia um byte para a porta de E/S */
void outb(unsigned short port, unsigned char data);

/** inb  – lê um byte da porta de E/S */
unsigned char inb(unsigned short port);

#endif
