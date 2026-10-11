
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>


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

//
// ISR define in interrupt.h
//
// #define ISR(vector, ...)                                                      \
//  void vector(void) __attribute__((__signal__, __INTR_ATTRS)) __VA_ARGS__;     \
//  void vector(void)
//

#define PRESSED 0
#define RISING 1

uint16_t date = 0;

uint16_t datecmp(uint16_t first, uint16_t second) {
	if (second > first) {
		return (second - first);
	} else {
		return (0xFFFF - first + second);
	}
}

int main() {
	
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB4);

	// Must set I-bit in SREG (Status register)
	// same as calling sei();
	SREG |= (1 << 7);

	// External Interrupt Mask Register
	// Allows SW1 to interrupt
	EIMSK |= (1 << INT0);

	// External Interrupt Control Register A
	// Interrupts on rising edge of INT0
	EICRA |= (1 << ISC01) | (1 << ISC01);

	//
	TCCR1B = (1 << CS12) | (1 << CS10);
	TCCR1A = 0;
	TCCR1C = 0;
	//15625 Hz

	while (1) {
		if (date == 0)
			continue;
		PORTB ^= (1 << PORTB1);
		if (datecmp(date, TCNT1) > 15625) {
			PORTB ^= (1 << PORTB0);
			date = 0;
		}
		_delay_ms(200);
	}
}


//Set INT0 interrupt's address
void INT0_vect(void) __attribute__((__signal__, __used__, __externally_visible__));
void INT0_vect(void) {
	PORTB ^= (1 << PORTB2);
	if (date != 0)
		return ;
	PORTB ^= (1 << PORTB4);
	date = TCNT1;
	// _delay_ms(1000);
	// //Toggle led
	// PORTB ^= (1 << PORTB0);
	EIFR |= (1 << INTF0);
}
