#include "utils.h"
#include <stdint.h>

int8_t ft_strncmp(char *s1, char *s2, uint8_t n) {
	uint8_t	i;

	i = 0;
	if (n < 1)
		return (0);
	while (s1[i] && s2[i] && (s1[i] == s2[i]) && i < (n - 1))
		i++;
	return ((unsigned char)s1[i] - (unsigned char)s2[i]);
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

