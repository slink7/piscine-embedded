#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "spi.h"


uint32_t colors[] = {
	0xE0000000,
	0xE0000000,
	0xE0000000,
	0xE1FFFF00,
	0xE0000000,
	0xE0000000,
};

int main(void) {
	spi_master_init();
	uint8_t at = 0;
	while ('0') {
		sk9822_set_leds(colors + 3 - ((3 + at++) % 4), 3);
		_delay_ms(250);
	}
}
