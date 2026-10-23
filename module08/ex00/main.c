#include <avr/io.h>

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

void sk9822_start() {
	for (uint8_t k = 0; k < 4; ++k) {
		spi_master_transmit(0x00);
	}
}

void sk9822_stop() {
	for (uint8_t k = 0; k < 4; ++k) {
		spi_master_transmit(0xFF);
	}
}

void sk9822_set_led(uint8_t r, uint8_t g, uint8_t b) {
	spi_master_transmit(0xE0 | 3);
	spi_master_transmit(b);
	spi_master_transmit(g);
	spi_master_transmit(r);
}

int main(void) {
	spi_master_init();

	sk9822_start();
	sk9822_set_led(0xff, 0x00, 0x00);
	sk9822_set_led(0x00, 0x00, 0x00);
	sk9822_set_led(0x00, 0x00, 0x00);
	sk9822_stop();

	while ('0')
		;
}
