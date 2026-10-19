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
		return (1);
	eeprom_force_write(addr, byte);
	return (eeprom_read(addr) == byte);
}

uint8_t eeprom_memcpy_w(uint16_t dst, void *src, uint8_t len) {
	if (dst + len > EELEN)
		return (0);
	for (uint8_t k = 0; k < len; ++k) {
		if (!eeprom_write(dst + k, ((uint8_t *)src)[k]))
			return (0);
	}

	return (1);
}

void eeprom_memcpy_r(uint16_t src, void *dst, uint8_t len) {
	if (src + len > EELEN)
		return ;
	for (uint8_t k = 0; k < len; ++k) {
		((uint8_t*)dst)[k] = eeprom_read(src + k);
	}
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

void eeprom_hexdump_C() {
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
