#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#ifndef F_CPU
	#warning "Missing -DF_CPU compilation flags"
	#define F_CPU 16000000UL
#endif

// ==========================
// LED WRAPPER
// ==========================

typedef struct {
	int ddrb_b;
	int pin_b;
} led;

led new_led(int ddrb_b, int pin_b) {
	DDRB |= (1 << ddrb_b);
	return ((led) {
		ddrb_b,
		pin_b
	});
}

void toggle_led(led *l) {
	PORTB ^= (1 << l->pin_b);
}

// ==========================
// main
// ==========================

led d1;

int main() {
	d1 = new_led(DDB1, PB1);

	//Mettre le timer en mode CTC
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	//Mettre le prescaler
	TCCR1B |= (1 << CS12) | (1 << CS10);

	OCR1A = F_CPU / 2048;

	TIMSK1 |= (1 << OCIE1A);

	sei();

	while (1) {
	}
}

ISR(TIMER1_COMPA_vect) {
	toggle_led(&d1);
}
