//Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima Atualizacao: 10-02-2026)
//Sinaliza o ponto de entrada do executavel -> loader
ENTRY(loader)

//Especifica o formato do arquivo de saida -> 32 bits para arquitetura x86
OUTPUT_FORMAT(elf32-i386)

SECTIONS {
    . = 0x1000;  /* Onde o bootloader nos carrega, onde o kernel eh carregado*/
    
    /*Partes do programa com diferentes propositos 
    (Cada parte do codigo vai ser dividida) e enquadrada aqui:*/

    //Instrucoes -> codigo executavel
    .text : {
        *(.text)
    }
    
    //Variaveis com valor
    .data : {
        *(.data)
    }
    
    //Variaveis sem valor de entrada
    .bss : {
        *(.bss)
    }
    
    //Partes desnecessarias -> comentarios...
    /DISCARD/ : {
        *(.comment)
        *(.eh_frame)
    }
}