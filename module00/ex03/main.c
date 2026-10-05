
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

int main() {
	//Allows writing to PB0 (for D1)
	//Data Direction Register B
	DDRB |= (1 << DDB0);

	//Allows reading from PD2 (for SW1)

	//Data Direction Register D
	DDRD &= ~(1 << PD2);

	int rising_edge = 0;
	int memory = 0;

	while (1) {
		//Actually reads from SW1 (PD2)
		uint8_t pin_state = !(PIND & (1 << PIND2));

		if (!rising_edge && pin_state) {
			//Bounce Protection
			_delay_ms(10);
			pin_state = !(PIND & (1 << PIND2));
			if (pin_state) {
				rising_edge = 1;
				memory = !memory;
				//Actually writes to D1 (PB0)
				PORTB = (!!(memory) << PB0);
			}
		}

		if (rising_edge && !pin_state) {
			rising_edge = 0;
		}
	}
}
