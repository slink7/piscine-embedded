#include <avr/io.h>

#ifndef F_CPU
	#warning "Missing -DF_CPU compilation flags"
	#define F_CPU 16000000UL
#endif

//Timer0 8bits
//Timer1 16bits

//COM1A1 => controls OC1A // Alternate function of PB1

// Alternate function Timer 1
// PB0 = ICP1
// PB1 = OC1A
// PB2 = OC1B
// PB4 = MISO?

int main() {

	//Sets PB1 to write
	DDRB |= (1 << PB1);
	// DDRB |= (1 << PB2);


	// Set Fast PWM with TOP = ICR1
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);


	//Non inverting mode for PB1
	TCCR1A |= (1 << COM1A1);
	// For PB2
	// TCCR1A |= (1 << COM1B1);

	
	//Prescaler 1024
	TCCR1B |= (1 << CS00) | (1 << CS02);

	//Set the period (TOP) to 1s (15625 TCNT1, prescaler 1024, 16MHz)
	ICR1 = F_CPU / 1024;
	//Set the toggle threshold
	OCR1A = ICR1 / 2;
	
	// For PB2
	// OCR1B = ICR1 / 4;

	while (1) {
	}
}
