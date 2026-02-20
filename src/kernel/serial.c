#include "serial.h"
#include "io.h"

#define SERIAL_DATA_PORT(base)          (base)
#define SERIAL_FIFO_COMMAND_PORT(base)  (base + 2)
#define SERIAL_LINE_COMMAND_PORT(base)  (base + 3)
#define SERIAL_MODEM_COMMAND_PORT(base) (base + 4)
#define SERIAL_LINE_STATUS_PORT(base)   (base + 5)

#define SERIAL_LINE_ENABLE_DLAB 0x80
#define SERIAL_FIFO_ENABLE      0xC7
#define SERIAL_MODEM_CONFIG     0x03

void serial_configure(unsigned short com)
{
    outb(SERIAL_LINE_COMMAND_PORT(com), SERIAL_LINE_ENABLE_DLAB);
    outb(SERIAL_DATA_PORT(com), 0x00);
    outb(SERIAL_DATA_PORT(com), 0x02);   // divisor = 2 → 57600 baud

    outb(SERIAL_LINE_COMMAND_PORT(com), 0x03);
    outb(SERIAL_FIFO_COMMAND_PORT(com), SERIAL_FIFO_ENABLE);
    outb(SERIAL_MODEM_COMMAND_PORT(com), SERIAL_MODEM_CONFIG);
}

int serial_is_transmit_fifo_empty(unsigned short com)
{
    return inb(SERIAL_LINE_STATUS_PORT(com)) & 0x20;
}

int serial_write(unsigned short com, char *buf, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++) {
        while (!serial_is_transmit_fifo_empty(com));
        outb(SERIAL_DATA_PORT(com), buf[i]);
    }
    return len;
}
