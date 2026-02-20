/*fb.c implementa o driver do framebuffer que controla a saida
de texto do console - feito por Arthur Ricartte (20/02/2026)*/

#include "fb.h"
#include "io.h"

// Endereço base do framebuffer na memória
static char *fb = (char *)0xB8000;

// Porta de comando = selecionar qual registrador do controlador de vídeo queremos acessar
#define FB_COMMAND_PORT 0x3D4

// Porta de dados = ler ou escrever o valor do registrador selecionado
#define FB_DATA_PORT 0x3D5

// Indice do registrador que armazena o byte baixo da posicao do cursor
#define FB_HIGH_BYTE_CMD 14

// Indice do registrador que armazena o byte alto da posicao do cursor
#define FB_LOW_BYTE_CMD 15

// Dimensoes da tela = 80 colunas e 25 linhas
#define FB_WIDTH 80
#define FB_HEIGHT 25

// Armazena a posicao atual do cursor:
static unsigned short cursor_pos = 0;

// Escreve um caractere em uma posicao especifica, com as cores especificadas na posicao i do framebuffer
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    // Cada caractere ocupa 2 bytes: o primeiro para o caractere e o segundo para as cores (4 bytes no total)
    fb[2 * i] = c;
    fb[2 * i + 1] = ((fg & 0x0F) << 4) | (bg & 0x0F);
    // 0x0F é uma mascara para garantir que apenas os 4 bits mais baixos sejam usados
}

// Move o cursor para a posicao especificada (0 a 1999)
void fb_move_cursor(unsigned short pos)
{
    /*A posicao eh de 16 bits mas as portas so transferem 8 bits
    Sendo necessario fazer duas escritas para transferir a posicao completa*/

    // Seleciona o byte alto
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);

    // Envia o byte alto da posicao para a porta de dados
    outb(FB_DATA_PORT, (pos >> 8) & 0xFF);

    // Seleciona o byte baixo
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);

    // Envia o byte baixo da posicao para a porta de dados
    outb(FB_DATA_PORT, pos & 0xFF);
}

// Rola a tela para cima quando o cursor chega no final
static void fb_scroll(void)
{
    int i, j;
    for (i = 1; i < FB_HEIGHT; i++)
    {
        for (j = 0; j < FB_WIDTH; j++)
        {
            unsigned int src = i * FB_WIDTH + j;
            unsigned int dest = (i - 1) * FB_WIDTH + j;
            fb[2 * dest] = fb[2 * src];
            fb[2 * dest + 1] = fb[2 * src + 1];
        }
    }
    for (j = 0; j < FB_WIDTH; j++)
    {
        unsigned int last = (FB_HEIGHT - 1) * FB_WIDTH + j;
        fb[2 * last] = ' ';
        fb[2 * last + 1] = (FB_WHITE << 4) | FB_BLACK;
    }
    cursor_pos -= FB_WIDTH;
}

int fb_write(char *buf, unsigned int len)
{
    for (unsigned int i = 0; i < len; i++)
    {
        char c = buf[i];
        if (c == '\n')
        {
            cursor_pos = (cursor_pos / FB_WIDTH + 1) * FB_WIDTH;
        }
        else
        {
            fb_write_cell(cursor_pos, c, FB_WHITE, FB_BLACK);
            cursor_pos++;
        }
        if (cursor_pos >= FB_WIDTH * FB_HEIGHT)
        {
            fb_scroll();
        }
        fb_move_cursor(cursor_pos);
    }
    return len;
}

void fb_clear(void)
{
    for (int i = 0; i < FB_WIDTH * FB_HEIGHT; i++)
    {
        fb_write_cell(i, ' ', FB_WHITE, FB_BLACK);
    }
    cursor_pos = 0;
    fb_move_cursor(0);
}
