bits 16
org 0x7C00

start:
    mov si, msg
    call print
    jmp $

print:
    lodsb
    or al, al
    jz .done
    mov ah, 0x0E
    int 0x10
    jmp print
.done:
    ret

msg db 'Hello, World!', 0

times 510-($-$$) db 0
dw 0xAA55
