bits 16
org 0x7C00

start:
    ; Setup
    xor ax, ax
    mov ds, ax
    mov es, ax
    mov ss, ax
    mov sp, 0x7C00

    ; Modo texto
    mov ax, 0x0003
    int 0x10

    ; Mensagem 1
    mov si, msg1
    call print

    ; Carregar kernel (2 setores)
    mov ah, 0x02
    mov al, 2
    mov ch, 0
    mov dh, 0
    mov cl, 2    ; setor 2
    mov bx, 0x1000
    int 0x13
    jc error

    ; Mensagem 2
    mov si, msg2
    call print

    ; Pausa para ver mensagem
    mov cx, 0xFFFF
.delay:
    nop
    loop .delay

    ; Pular para kernel
    jmp 0x1000

error:
    mov si, msg_err
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

msg1 db "Boot: ", 0
msg2 db " OK! Indo para kernel...", 0x0D, 0x0A, 0
msg_err db " ERRO", 0

times 510-($-$$) db 0
dw 0xAA55
