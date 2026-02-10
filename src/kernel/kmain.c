void kmain(void) {
    char *video_memory = (char *) 0xB8000;
    
    // escreve "OS" em verde no canto da tela
    video_memory[0] = 'O';
    video_memory[1] = 0x02; // verde escuro
    video_memory[2] = 'S';
    video_memory[3] = 0x02; // verde escuro
}