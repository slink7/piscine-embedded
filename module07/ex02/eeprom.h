#ifndef EEPROM_H
#define EEPROM_H

#include <avr/io.h>
#include <stdint.h>

#include "uart.h"

uint8_t eeprom_read(uint16_t addr);
void eeprom_force_write(uint16_t addr, uint8_t byte);
uint8_t eeprom_write(uint16_t addr, uint8_t byte);

#define EELEN	1024u
#define BPL		24u
#define LINES	(EELEN / BPL)

void eeprom_hexdump(uint16_t ha);

#endif
