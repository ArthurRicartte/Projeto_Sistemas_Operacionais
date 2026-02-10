ENTRY(main)
OUTPUT_FORMAT(binary)

SECTIONS
{
    /* O código começa no endereço 0x1000 */
    . = 0x1000;

    .text : {
        *(.text)
    }

    .data : {
        *(.data)
    }

    .bss : {
        *(.bss)
    }
}