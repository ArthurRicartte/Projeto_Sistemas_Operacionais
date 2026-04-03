#ifndef INCLUDE_FB_H
#define INCLUDE_FB_H

// Cores (0-15) – conforme tabela do livro
#define FB_BLACK 0
#define FB_BLUE 1
#define FB_GREEN 2
#define FB_CYAN 3
#define FB_RED 4
#define FB_MAGENTA 5
#define FB_BROWN 6
#define FB_LIGHT_GREY 7
#define FB_DARK_GREY 8
#define FB_LIGHT_BLUE 9
#define FB_LIGHT_GREEN 10
#define FB_LIGHT_CYAN 11
#define FB_LIGHT_RED 12
#define FB_LIGHT_MAGENTA 13
#define FB_LIGHT_BROWN 14
#define FB_WHITE 15

/** fb_write_cell – escreve um caractere na posição i do framebuffer */
void fb_write_cell(unsigned int i, char c, unsigned char fg, unsigned char bg);

/** fb_move_cursor – move o cursor para a posição pos (0 a 1999) */
void fb_move_cursor(unsigned short pos);

/** fb_write – escreve uma string no framebuffer (avança cursor, faz rolagem) */
int fb_write(char *buf, unsigned int len);

/** fb_clear – limpa toda a tela */
void fb_clear(void);

/** fb_get_cursor – retorna a posição atual do cursor */
unsigned short fb_get_cursor(void);

/** fb_set_cursor – define a posição do cursor */
void fb_set_cursor(unsigned short pos);

/** fb_delete_char – apaga o caractere anterior ao cursor */
void fb_delete_char(void);

/** Navegação de Scroll (Setas do Teclado) */
void fb_scroll_up(void);
void fb_scroll_down(void);

#endif
