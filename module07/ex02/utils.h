#ifndef UTILS_H
#define UTILS_H

#include <avr/io.h>

int ft_strncmp(char *a, char *b, uint8_t n);
uint8_t ft_strlen(char *str);
void ft_memcpy(void *dst, void *src, uint8_t len);

#endif
