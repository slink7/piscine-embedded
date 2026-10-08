
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

//
// ISR define in interrupt.h
//
// #define ISR(vector, ...)                                                      \
//  void vector(void) __attribute__((__signal__, __INTR_ATTRS)) __VA_ARGS__;     \
//  void vector(void)
//

int main() {
	
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);

	//Timer1, mode 7. FastPWM, TOP = OCRA
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);

	//Prescaler 1024
	TCCR1B |= (1 << CS10) | (1 << CS10);

	//PB1
	TCCR1A |= (1 << COM1A1);

	ICR1 = 30;

	OCR1A = 0;


	// ===============================
	//
	// ===============================


	// Timer0 mode Fast PWM
	TCCR0A |= (1 << WGM02) | (1 << WGM00) | (1 << WGM00);

	TCCR0B |= (1 << CS02) | (1 << CS00);

	// 16e6 / (1024 * 255) = 61Hz
	// 61 Interrupts per seconds
	OCR0A = 0xFF;

	//Set interrupt
	TIMSK0 |= (1 << OCIE0A);

	// Must set I-bit in SREG (Status register)
	// same as calling sei();
	SREG |= (1 << 7);

	while (1) {}
}

int8_t dir = 2;

//Set OCIE0A interrupt's address
void TIMER0_COMPA_vect(void) __attribute__((__signal__, __INTR_ATTRS));
void TIMER0_COMPA_vect(void) {
	OCR1A += dir;
	if (OCR1A >= ICR1 || OCR1A <= 0)
		dir *= -1;
}
