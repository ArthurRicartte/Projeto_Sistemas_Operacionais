;Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima Atualizacao: 10-02-2026)
bits 16
org 0x7C00

KERNEL_OFFSET equ 0x1000  ;Onde o kernel será carregado

start:
    ;Setup básico
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ;Modo texto 80x25
    mov ax, 0x0003
    int 0x10

    ;Mensagem inicial
    mov si, msg_loading
    call print

    ;Carregar mais setores para o kernel (pelo menos 10)
    mov ah, 0x02      ; Função de leitura
    mov al, 10        ; Número de setores (ajuste conforme necessário)
    mov ch, 0         ; Cilindro 0
    mov dh, 0         ; Cabeça 0
    mov cl, 2         ; Setor 2 (após o bootloader)
    mov bx, KERNEL_OFFSET
    int 0x13
    jc disk_error

    ;Mensagem de sucesso
    mov si, msg_ok
    call print

    ;Mudar para modo protegido
    cli                      ; Desabilitar interrupções
    lgdt [gdt_descriptor]    ; Carregar GDT

    ;Habilitar modo protegido
    mov eax, cr0
    or eax, 0x1
    mov cr0, eax

    ;Pular para modo protegido
    jmp CODE_SEG:init_protected_mode

disk_error:
    mov si, msg_error
    call print
    hlt

print:
    lodsb
    test al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

; GDT (Global Descriptor Table)
gdt_start:
    dq 0x0                ;Descritor nulo

gdt_code:
    dw 0xFFFF             ;Limite (0-15)
    dw 0x0                ;Base (0-15)
    db 0x0                ;Base (16-23)
    db 10011010b          ;Flags
    db 11001111b          ;Flags + Limite (16-19)
    db 0x0                ;Base (24-31)

gdt_data:
    dw 0xFFFF
    dw 0x0
    db 0x0
    db 10010010b
    db 11001111b
    db 0x0

gdt_end:

gdt_descriptor:
    dw gdt_end - gdt_start - 1
    dd gdt_start

CODE_SEG equ gdt_code - gdt_start
DATA_SEG equ gdt_data - gdt_start

bits 32
init_protected_mode:
    ;Configurar segmentos de dados
    mov ax, DATA_SEG
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    mov esp, 0x9000       ;Pilha em 0x9000

    ;Pular para o kernel
    jmp CODE_SEG:KERNEL_OFFSET

bits 16
msg_loading db "Carregando kernel... ", 0
msg_ok db "OK!", 0x0D, 0x0A, 0
msg_error db "Erro disco!", 0

times 510-($-$$) db 0
dw 0xAA55