#ifndef INCLUDE_SERIAL_H
#define INCLUDE_SERIAL_H

#define SERIAL_COM1 0x3F8

void serial_configure(unsigned short com);
int serial_write(unsigned short com, char *buf, unsigned int len);
int serial_is_transmit_fifo_empty(unsigned short com);

#endif
