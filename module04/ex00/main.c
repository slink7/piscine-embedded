
#include <avr/io.h>
#include <avr/interrupt.h>

//
// D1 / PB0
//
// ICP1 (TC1 Input capture input0)
// CLKO (Divided system clock Output)
// PCINT0 (Pin Change Interrupt 0)
//

//
// SW1 / PD2
//
// INT0 (External Interrupt 0 Input)
// PCINT18 (Pin Change Interrupt 18)
//

int main() {
	
	DDRB |= (1 << PB0);

	// Must set I-bit in SREG (Status register)
	// same as calling sei();
	SREG |= 1 << 7;

	// External Interrupt Mask Register
	//
	EIMSK |= (1 << INT0);

	// External Interrupt Control Register A
	// Interrupts on rising edge of INT0
	EICRA |= (1 << ISC01) | (1 << ISC00);

	while (1) {}
}

//Set INT0 interrupt's address
ISR(INT0_vect) {
	//Toggle led
	PORTB ^= (1 << PORTB0);
}
