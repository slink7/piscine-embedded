#include "eeprom.h"
#include <stdint.h>

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

void eeprom_force_write(uint16_t addr, uint8_t byte) {
	
	// Wait for previous write
	while (EECR & (1 << EEPE))
		;
	// Setup data
	EEAR = addr;
	EEDR = byte;

	//
	EECR |= (1 << EEMPE);

	// Start write mode
	EECR |= (1 << EEPE);
}

uint8_t eeprom_write(uint16_t addr, uint8_t byte) {
	if (eeprom_read(addr) == byte)
		return (0);
	eeprom_force_write(addr, byte);
	return (1);
}

void eeprom_hexdump(uint16_t ha) {
	for (uint8_t l = 0; l < LINES; ++l) {
		uart_print_ui32(BPL * l);
		uart_tx(' ');

		for (uint8_t k = 0; k < BPL; ++k) {
			if (ha == (l * BPL + k))
				uart_printstr("\e[31m");
			uart_print_byte(eeprom_read(l * BPL + k));
			if (ha == (l * BPL + k))
				uart_printstr("\e[0m");
			if (k % 2 == 1)
				uart_tx(' ');
		}

		uart_printstr("\r\n");
	}
	uart_print_ui32(BPL * LINES);
	uart_printstr("\r\n");
}
