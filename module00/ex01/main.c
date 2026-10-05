
#include <avr/io.h>

int main() {
	//Allows writing to PB0
	//Data Direction Register B 
	DDRB |= (1 << DDB0);

	//Actually toggles on PB0
	PORTB |= (1 << PB0);
}
