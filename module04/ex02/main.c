
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
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB4);

	DDRD &= ~(1 << DDD4);


	EIMSK |= (1 << INT0);
	EICRA |= (1 << ISC00) | (1 << ISC00);

	PCMSK2 |= (1 << PCINT20);
	PCICR |= (1 << PCIE2);

	SREG |= (1 << 7);

	while (1) {}
}

MY_ISR(INT0_vect) {
	_delay_ms(50.0);
	if ((PIND & (1 << PIND2)))
		return ;
	PORTB++;
	PORTB = (PORTB & ~(1 << PORTB4)) | (!!(PORTB & (1 << PORTB3)) << PORTB4);
}

	// PORTB = (PORTB & ~(1 << l->pin_b)) | (!!v << l->pin_b);
MY_ISR(PCINT2_vect) {
	_delay_ms(50.0);
	if ((PIND & (1 << PIND4)))
		return ;
	PORTB--;
	PORTB = (PORTB & ~(1 << PORTB4)) | (!!(PORTB & (1 << PORTB3)) << PORTB4);
}
