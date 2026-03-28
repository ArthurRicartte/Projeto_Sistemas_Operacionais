#ifndef STRING_H
#define STRING_H

// Copia string
static inline void strcpy(char *dest, const char *src)
{
    while (*src)
    {
        *dest++ = *src++;
    }
    *dest = '\0';
}

// Retorna tamanho da string
static inline int strlen(const char *str)
{
    int len = 0;
    while (str[len])
        len++;
    return len;
}

#endif