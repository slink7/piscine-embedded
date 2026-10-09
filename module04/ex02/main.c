
#include <avr/io.h>
#include <util/delay.h>
// #include <avr/interrupt.h>

//
// ISR define in interrupt.h
//
#define MY_ISR(vector, ...)                                                      \
	void vector(void) __attribute__((__signal__)) __VA_ARGS__;     \
	void vector(void)

int main() {

	//Set leds as outputs
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB4);

	//Set buttons as inputs
	DDRD &= ~(1 << DDD2);
	DDRD &= ~(1 << DDD4);

	//External Interrupt Mask Register
	//Allows the PD2 (SW1) to interrupt (secondary function)
	EIMSK |= (1 << INT0);
	//External Interrupt Control Register A
	// Sets the control to: interrupt on any logical change on INT0
	EICRA |= (1 << ISC00);

	//Pin Change Mask Register
	//Allows PD4 to interrupt (secondary function)
	PCMSK2 |= (1 << PCINT20);
	//Pin Change Interrupt Control Register
	//Allows interrupt on any change on PCINT[23:16]
	PCICR |= (1 << PCIE2);

	//Allows global interrupt (sei())
	SREG |= (1 << 7);

	while (1) {}
}

MY_ISR(INT0_vect) {
	_delay_ms(50.0);
	if ((PIND & (1 << PIND2)))
		return ;
	PORTB++;
	// Copy 4th bit to the 5th
	PORTB = (PORTB & ~(1 << PORTB4)) | (!!(PORTB & (1 << PORTB3)) << PORTB4);
}

	// PORTB = (PORTB & ~(1 << l->pin_b)) | (!!v << l->pin_b);
MY_ISR(PCINT2_vect) {
	_delay_ms(50.0);
	if ((PIND & (1 << PIND4)))
		return ;
	PORTB--;
	// Copy 4th bit to the 5th
	PORTB = (PORTB & ~(1 << PORTB4)) | (!!(PORTB & (1 << PORTB3)) << PORTB4);
}
