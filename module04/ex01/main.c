
#include <avr/io.h>
#include <stdint.h>

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

#define S(BIT) (1 << BIT)

uint16_t counter = 0;

#define MIN(a, b) (a + (a > b) * (b - a))
#define F(x) (2 * MIN(x, 30 - x))

int main() {

	DDRB |= S(DDB1);
	
	// Mode 1, PWM Phase Correct, TOP = 0x00FF
	TCCR1A |= S(WGM10) | S(WGM10);

	// No Prescaler
	TCCR1B |= S(CS10);

	// PB1
	TCCR1A |= S(COM1A1) | S(COM1A1);

	// TOP = 0x00FF;
	// Initial Duty Cycle = 1%
	OCR1A = 1 * 0x00FF / 100;




	//Mode 1, PWM Phase Correct, TOP = 0xFF
	TCCR0A |= S(WGM00) | S(WGM00);

	// Prescaler 1024
	TCCR0B |= S(CS02) | S(CS00);
	// Effective Freq: 16e6 / 1024 = 15625
	
	//TOP = 0xFF
	//Interrupts at 15625 / 255 = 61Hz
	// Half to rise, half to fall: 30


	//Set interrupt
	TIMSK0 |= (1 << OCIE0A);

	// Must set I-bit in SREG (Status register)
	// same as calling sei();
	SREG |= (1 << 7);


	while (1) {
		OCR1A = (100 * F(counter % 30) / 30) * 0x00FF / 100;
	}
}

//Set OCIE0A interrupt's address
void TIMER0_COMPA_vect(void) __attribute__((__signal__, __used__));
void TIMER0_COMPA_vect(void) {
	counter++;
}
