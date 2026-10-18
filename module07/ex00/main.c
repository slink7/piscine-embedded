#include <avr/io.h>
#include <stdint.h>

#include "uart.h"

uint8_t eeprom_read(uint16_t addr) {
	// Wait for previous write
	while (EECR & (1 << EEPE))
		;
	// Set address
	EEAR = addr;
	// Start read mode
	EECR |= (1 << EERE);
	
	return (EEDR);
}

#define EELEN	1024
#define BPL		16
#define LINES	(EELEN / BPL)

int main(void) {
	
	uart_init(UBRR);

	uint8_t line[BPL] = {};
	for (uint8_t l = 0; l < LINES; ++l) {
		uart_print_ui32(BPL * l);
		uart_printstr("  ");

		for (uint8_t k = 0; k < BPL; ++k)
			line[k] = eeprom_read(l * BPL + k);

		for (uint8_t k = 0; k < BPL; ++k) {
			uart_print_byte(line[k]);
			uart_printstr((k % 8 == 7) ? "  " : " ");
		}

		uart_tx('|');
		for (uint8_t k = 0; k < BPL; ++k)
			uart_print_pchar(line[k]);
		uart_printstr("|\r\n");
	}
	uart_print_ui32(BPL * LINES);
}

