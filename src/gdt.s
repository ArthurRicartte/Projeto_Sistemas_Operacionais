; gdt.s - Carrega a GDT e atualiza os registradores de segmento
global gdt_flush

gdt_flush:
    ; O compilador C passa o argumento (o ponteiro gdt_ptr) na pilha.
    ; Lemos esse endereço para o registrador eax.
    mov eax, [esp+4]
    
    ; Carrega a nova GDT
    lgdt [eax]

    ; Agora precisamos atualizar os registradores de segmento de dados.
    ; O nosso Segmento de Dados é a entrada 2 da GDT. 
    ; Cada entrada tem 8 bytes, então 2 * 8 = 16 (que em hexadecimal é 0x10).
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ; Para atualizar o registrador de Código (CS), fazemos um "Far Jump" (salto longo).
    ; O nosso Segmento de Código é a entrada 1. (1 * 8 = 8, ou 0x08 em hex).
    jmp 0x08:.flush

.flush:
    ret