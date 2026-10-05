
#include <avr/io.h>
#include <stdint.h>

int main() {
	//Allows writing to PB0 (for D1)
	//Data Direction Register B
	DDRB |= (1 << DDB0);

	//Allows reading from PD2 (for SW1)

	//Data Direction Register D
	DDRD &= ~(1 << PD2);

	while (1) {
		//Actually reads from SW1 (PD2)
		uint8_t pinState = !(PIND & (1 << PIND2));

		//Actually writes to D1 (PB0)
		PORTB = (!!(pinState) << PB0);
	}
}
