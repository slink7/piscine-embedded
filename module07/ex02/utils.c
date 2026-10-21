#include "utils.h"
#include <stdint.h>

int ft_strncmp(char *a, char *b, uint8_t n) {
	while (*a && *b && *a == *b && n > 0) {
		a++;
		b++;
		n--;
	}
	return (*b - *a);
}

uint8_t ft_strlen(char *str) {
	uint8_t out = 0;
	while (str[out])
		out++;
	return (out);
}

void ft_memcpy(void *dst, void *src, uint8_t len) {
	for (uint8_t k = 0; k < len; ++k)
		((uint8_t*)dst)[k] = ((uint8_t*)src)[k];
}

