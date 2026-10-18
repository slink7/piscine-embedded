#include <avr/io.h>
#include <stdint.h>

#include "uart.h"
#include "eeprom.h"

#define INPUT_SIZE 6

uint8_t from_hex(char c) {
	return (
		(c >= '0' && c <= '9')
		? c - '0'
		: (c >= 'A' && c <= 'F')
		? c - 'A' + 10
		: 0xFF
	);
}

// REGEX
//^[0-F]{1,3} [0-F]{2}$
uint8_t parse_input(uint8_t *in, uint16_t *addr, uint8_t *value) {
	uint8_t at = 0;
	uint8_t nibble = 0;

	*addr = 0;
	do {
		nibble = from_hex(in[at]);
		if (nibble == 0xFF) {
			if (at != 0)
				break ;
			return (0);
		}
		*addr = *addr << 4 | nibble;
		at++;
	} while (1);

	if (in[at++] != ' ')
		return (0);

	*value = 0;
	for (uint8_t k = 0; k < 2; ++k) {
		nibble = from_hex(in[at]);
		if (nibble == 0xFF)
			return (0);
		*value = *value << 4 | nibble;
		at++;
	}

	return (1);
}

int main(void) {
	
	uart_init(UBRR);

	uint8_t input[INPUT_SIZE + 1] = {};
	uint16_t addr;
	uint8_t value;
	while (1) {
		uart_printstr("=> ");
		uart_read(input, INPUT_SIZE);
		uart_printstr("\n\r");
		if (!parse_input(input, &addr, &value)) {
			uart_printstr("Invalid input.\n\r");
			continue ;
		}
		if (addr >= EELEN) {
			uart_printstr("Address is too big.\n\r");
			continue ;
		}
		if (eeprom_write(addr, value))
			eeprom_hexdump(addr);
	}
}
