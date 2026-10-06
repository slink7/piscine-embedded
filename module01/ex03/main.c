#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
	#warning "Missing -DF_CPU compilation flags"
	#define F_CPU 16000000UL
#endif

// ============================
// BUTTON WRAPPER
// ============================

const double DEBOUCER_DELAY = 10.0;

typedef struct {
	uint8_t prev;
	int ddrd_b;
	int pin_b;
} button;

button new_button(int ddrd_b, int pin_b) {
	DDRD &= ~(1 << ddrd_b);
	return ((button) {
		0,
		ddrd_b,
		pin_b
	});
}

int is_rising(button *b) {
	uint8_t value = !(PIND & (1 << b->pin_b));
	_delay_ms(DEBOUCER_DELAY);
	if (value != !(PIND & (1 << b->pin_b)))
		return (0);
	int out = value && !b->prev;
	b->prev = value;
	return (out);
}

int clamp(int val, int min, int max) {
	return (val < min ? min : val > max ? max : val);
}

int main() {

	/* TIMER1 SETUP */


	//Sets PB1 to write
	DDRB |= (1 << PB1);


	// Set Fast PWM with TOP = ICR1
	TCCR1A |= (1 << WGM11);
	TCCR1B |= (1 << WGM13) | (1 << WGM12);


	//Non inverting mode for PB1
	TCCR1A |= (1 << COM1A1);

	
	//Prescaler 1024
	TCCR1B |= (1 << CS00) | (1 << CS02);

	//Set the period (TOP) to 1s (15625 TCNT1, prescaler 1024, 16MHz)
	ICR1 = F_CPU / 1024;
	//Set the toggle threshold
	OCR1A = ICR1 / 10;
	

	/* BUTTON SETUP */

	button sw1 = new_button(PD2, PIND2);
	button sw2 = new_button(PD4, PIND4);

	int step = ICR1 / 10;
	int select = 1;

	while (1) {
		select = clamp(
			select
			+ is_rising(&sw1)
			- is_rising(&sw2),
			1, 10
		);
		OCR1A = step * select;
	}
}
