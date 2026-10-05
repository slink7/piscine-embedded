
#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

int clamp(int val, int min, int max) {
	return (val < min ? min : val > max ? max : val);
}

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

void set_led(led *l, int v) {
	PORTB = (PORTB & ~(1 << l->pin_b)) | (!!v << l->pin_b);
}

// ==========================
// main
// ==========================

int main() {
	led leds[] = { 
		new_led(DDB0, PB0),
		new_led(DDB1, PB1),
		new_led(DDB2, PB2),
		new_led(DDB4, PB4)
	};

	button sw1 = new_button(PD2, PIND2);
	button sw2 = new_button(PD4, PIND4);

	int memory = 0;

	while (1) {
		memory = clamp(
			memory
			+ is_rising(&sw1)
			- is_rising(&sw2),
			0, 15
		);

		for (int k = 0; k < 4; ++k) {
			int bit = 1 & (memory >> k);
			set_led(leds + k, bit);
		}
	}
}
