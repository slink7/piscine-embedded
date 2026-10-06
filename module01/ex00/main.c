#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

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

int main() {
	led d1 = new_led(DDB1, PB1);

	volatile unsigned long ticks = 0;

	while (1) {
		if (ticks % 10000 == 0)
			toggle_led(&d1);
		ticks++;
	}
}
