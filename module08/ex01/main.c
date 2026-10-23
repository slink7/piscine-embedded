#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "spi.h"


uint32_t colors[] = {
	0xE10000FF,
	0xE100FF00,
	0xE1FF0000,
	0xE100FFFF,
	0xE1FFFF00,
	0xE1FF00FF,
	0xE1FFFFFF
};

int main(void) {
	spi_master_init();
	uint8_t at = 0;
	while ('0') {
		sk9822_start();
		sk9822_set_led(colors[at++]);
		sk9822_set_led(0xE0000000);
		sk9822_set_led(0xE0000000);
		sk9822_stop();
		_delay_ms(1000);
	}
}
