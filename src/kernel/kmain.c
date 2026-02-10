// Nucleo do Sistema Operacional
// Desenvolvido por: Arthur Ricartte e Joao Veloso (Ultima Atualizacao: 10-02-2026)
void kmain(void)
{
    // Acesso direto à memória de vídeo:
    volatile char *video = (volatile char *)0xB8000;

    // Limpar tela:
    for (int i = 0; i < 80 * 25 * 2; i += 2)
    {
        video[i] = ' ';
        video[i + 1] = 0x07; // Cinza claro
    }

    // Escrever mensagem:
    const char *msg = "Kernel em Modo Protegido Funcionando!";
    int offset = 80 * 2; // Segunda linha

    for (int i = 0; msg[i] != '\0'; i++)
    {
        video[offset + i * 2] = msg[i];
        video[offset + i * 2 + 1] = 0x0A; // Verde claro
    }

    // Loop infinito:
    while (1)
    {
        // Por enquando vou deixar vazio!
    }
}