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

// Buffer de Scrollback (200 linhas de 80 colunas)
#define FB_BUFFER_LINES 200
static unsigned short fb_buffer[FB_BUFFER_LINES * FB_WIDTH];
static int scroll_offset = 0; // Quantas linhas para cima estamos olhando (0 = final)

// Inicializa o buffer com espaços em branco
static void fb_init_buffer(void)
{
    for (int i = 0; i < FB_BUFFER_LINES * FB_WIDTH; i++)
    {
        fb_buffer[i] = (unsigned short)(( (FB_WHITE & 0x0F) << 12) | ( (FB_BLACK & 0x0F) << 8) | ' ');
    }
}

// Atualiza a tela de vídeo (0xB8000) a partir do buffer interno
static void fb_refresh(void)
{
    // A janela de exibição (25 linhas) termina em (cursor_pos / 80)
    // Mas se estivermos com scroll_offset > 0, subimos essa janela.
    
    int current_line = cursor_pos / FB_WIDTH;
    int start_line = current_line - FB_HEIGHT + 1 - scroll_offset;
    
    if (start_line < 0) start_line = 0;

    for (int row = 0; row < FB_HEIGHT; row++)
    {
        for (int col = 0; col < FB_WIDTH; col++)
        {
            int buf_idx = (start_line + row) * FB_WIDTH + col;
            int screen_idx = row * FB_WIDTH + col;
            
            unsigned short val = fb_buffer[buf_idx];
            fb[2 * screen_idx] = (char)(val & 0xFF);
            fb[2 * screen_idx + 1] = (char)((val >> 8) & 0xFF);
        }
    }
    
    // Posiciona o cursor de hardware (apenas se estivermos na tela certa)
    if (scroll_offset == 0) {
        fb_move_cursor(cursor_pos % (FB_WIDTH * FB_HEIGHT)); 
    } else {
        fb_move_cursor(2000); // Esconde o cursor fora da tela
    }
}

// Escreve um caractere em uma posicao especifica no buffer apenas
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg)
{
    if (i >= FB_BUFFER_LINES * FB_WIDTH) return;
    fb_buffer[i] = (unsigned short)(( (fg & 0x0F) << 12) | ( (bg & 0x0F) << 8) | (unsigned char)c);
}

// Move o cursor para a posicao especificada (0 a 1999) - Apenas hardware
void fb_move_cursor(unsigned short pos)
{
    outb(FB_COMMAND_PORT, FB_HIGH_BYTE_CMD);
    outb(FB_DATA_PORT, (pos >> 8) & 0xFF);
    outb(FB_COMMAND_PORT, FB_LOW_BYTE_CMD);
    outb(FB_DATA_PORT, pos & 0xFF);
}

// Rola o buffer se necessário (Simplificado: apenas avança o cursor no buffer)
static void fb_scroll(void)
{
    // Se o cursor estourar o buffer, limpamos a metade superior e subimos tudo
    if (cursor_pos >= FB_BUFFER_LINES * FB_WIDTH)
    {
        int mid = (FB_BUFFER_LINES / 2) * FB_WIDTH;
        for (int i = 0; i < mid; i++) {
            fb_buffer[i] = fb_buffer[i + mid];
        }
        for (int i = mid; i < FB_BUFFER_LINES * FB_WIDTH; i++) {
            fb_buffer[i] = (unsigned short)(( (FB_WHITE & 0x0F) << 12) | ( (FB_BLACK & 0x0F) << 8) | ' ');
        }
        cursor_pos -= mid;
    }
}

int fb_write(char *buf, unsigned int len)
{
    static int initialized = 0;
    if (!initialized) { fb_init_buffer(); initialized = 1; }

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

        fb_scroll();
    }
    
    // Sempre que escrevemos, resetamos o scroll para o fim e redesenhamos
    scroll_offset = 0;
    fb_refresh();
    return len;
}

void fb_clear(void)
{
    fb_init_buffer();
    cursor_pos = 0;
    scroll_offset = 0;
    fb_refresh();
}

unsigned short fb_get_cursor(void)
{
    return cursor_pos;
}

void fb_set_cursor(unsigned short pos)
{
    cursor_pos = pos;
    fb_refresh();
}

void fb_delete_char(void)
{
    if (cursor_pos > 0)
    {
        cursor_pos--;
        fb_write_cell(cursor_pos, ' ', FB_WHITE, FB_BLACK);
        fb_refresh();
    }
}

void fb_scroll_up(void)
{
    int current_line = cursor_pos / FB_WIDTH;
    if (current_line > FB_HEIGHT + scroll_offset) {
        scroll_offset++;
        fb_refresh();
    }
}

void fb_scroll_down(void)
{
    if (scroll_offset > 0) {
        scroll_offset--;
        fb_refresh();
    }
}