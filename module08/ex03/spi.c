#include "spi.h"
#include <stdint.h>

void spi_master_init() {
	
	DDRB = DDRB
		// SS
		| (1 << DDB2)
		// MOSI
		| (1 << DDB3)
		// SCK
		| (1 << DDB5)
	;
	
	SPCR = SPCR
		// Enable
		| (1 << SPE)
		// Master
		| (1 << MSTR)
		// Clock 1/16
		| (0 << SPR1) | (1 << SPR0)
	;
	SPSR = 0;
}

void spi_master_transmit(uint8_t data) {
	// Write byte to send
	SPDR = data;

	// Wait for transmission to end
	while (!(SPSR & (1 << SPIF)))
		;
}

void spi_master_transmit_u32(uint32_t data) {
	for (uint8_t k = 0; k < 4; ++k)
		spi_master_transmit(data >> (8 * (3 - k)));
}

void sk9822_start() {
	spi_master_transmit_u32(0x00000000);
}

void sk9822_stop() {
	spi_master_transmit_u32(0xFFFFFFFF);
}

void sk9822_set_led(uint32_t led) {
	spi_master_transmit_u32(led);
}

void sk9822_set_leds(uint32_t *leds, uint8_t len) {
	sk9822_start();
	for (uint32_t k = 0; k < len; ++k)
		sk9822_set_led(leds[k]);
	sk9822_stop();
}

void sk9822_set_rgb(uint8_t brightness, uint8_t r, uint8_t g, uint8_t b) {
	spi_master_transmit(0xE0 | (0x1F && brightness));
	spi_master_transmit(b);
	spi_master_transmit(g);
	spi_master_transmit(r);
}
