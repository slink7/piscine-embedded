
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

// PD3  PD5 PD6
// blue red green

int main() {
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);

	uint8_t colors[] = {
		(1 << PD5),
		(1 << PD6),
		(1 << PD3)
	};

	uint8_t k = 0;
	while (1) {
		_delay_ms(1000.0);
		PORTD = colors[k++ % (sizeof(colors) / sizeof(uint8_t))];
	}
}
