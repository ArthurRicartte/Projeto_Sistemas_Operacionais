// Primeira versao do Kernel - feita por Arthur Ricartte
//  Kernel mínimo de teste - modo real (16-bit)
//  Vamos começar simples e depois migrar para modo protegido

void kernel_main()
{
    // Acessar memória de vídeo diretamente (modo real)
    char *video = (char *)0xB8000;

    // Escrever "KERNEL OK" colorido
    video[0] = 'K';
    video[1] = 0x0A; // Verde
    video[2] = 'E';
    video[3] = 0x0A;
    video[4] = 'R';
    video[5] = 0x0A;
    video[6] = 'N';
    video[7] = 0x0A;
    video[8] = 'E';
    video[9] = 0x0A;
    video[10] = 'L';
    video[11] = 0x0A;
    video[12] = ' ';
    video[13] = 0x0A;
    video[14] = 'O';
    video[15] = 0x0A;
    video[16] = 'K';
    video[17] = 0x0A;
    video[18] = '!';
    video[19] = 0x0A;

    // Loop infinito
    while (1)
        ;
}
