global enable_paging
enable_paging:
    ; Argumento: endereço do diretório em [esp+4]
    mov eax, [esp+4]
    mov cr3, eax

    ; Habilita PSE (Page Size Extensions) para páginas de 4 MiB
    mov ebx, cr4
    or  ebx, 0x00000010
    mov cr4, ebx

    ; Ativa a paginação (bit PG no CR0)
    mov ebx, cr0
    or  ebx, 0x80000000
    mov cr0, ebx

    ; Invalida TLB para o endereço virtual 0 (opcional)
    invlpg [0]

    ; Far jump para limpar o pipeline (seletor 0x08 = segmento de código do kernel)
    ; Ajuste o seletor se necessário (de acordo com sua GDT)
    jmp 0x08:clean_pipeline
clean_pipeline:
    ret