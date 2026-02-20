/* Desenvolvido por: Arthur Ricartte e Joao Veloso */
/* Ponto de entrada do kernel */
ENTRY(loader)

/* Formato 32 bits */
OUTPUT_FORMAT(elf32-i386)

SECTIONS
{
    /* O kernel é carregado em 0x1000 (1KB) */
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
