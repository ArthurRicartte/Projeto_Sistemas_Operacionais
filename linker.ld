/* 
Feito por Arthur Ricartte
Linker script para modo real (16-bit) 
Sera importante quando for migrar o Kernel para a Linguagem c*/

//Define o ponto inicial para rodar o programa:
ENTRY(kernel_main)

//Define o formato do arquivo de saida:
OUTPUT_FORMAT(binary)

SECTIONS {
    /* O kernel é carregado no endereco 0x1000 pelo bootloader */
    . = 0x1000;
    
    //Para as secoes .text (Codigo executavel) serao enviadas
    .text : {
        *(.text)
    }
    
    //Para as secoes .data (Dados) serao enviadas
    .data : {
        *(.data)
    }
    
    //Para as secoes .bss (Dados nao inicializados) serao enviadas
    .bss : {
        *(.bss)
    }
    
    //Comentarios e metadados serao descartados para deixar o binario menor
    /DISCARD/ : {
        *(.comment)
        *(.note.*)
    }
}
