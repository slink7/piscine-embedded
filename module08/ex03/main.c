#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#include "adc.h"
#include "spi.h"

#define LIT		0xE1FFFF00
#define UNLIT	0xE0000000

int main(void) {

	spi_master_init();
	adc_init();

	while ('0') {
	
		uint16_t rv1 = adc_read();

		sk9822_start();
		for (uint8_t k = 0; k < 3; ++k)
			sk9822_set_led((rv1 >= ((k + 1) * 341)) ? LIT : UNLIT);
		sk9822_stop();
	}
}
