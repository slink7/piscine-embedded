
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

// PD6
// OC0A (T/C0 output compare match B output)
//
// PD5
// OC0B (T/C0 Output compare match B output)
//
// PD3
// OC2B (T/C2 output compare match B output)

//14 1 1 1 0 Fast PWM ICR1 BOTTOM TOP

void init_rgb() {
	DDRD |= (1 << PD3) | (1 << PD5) | (1 << PD6);
	
	//Timer0, mode 7. FastPWM, TOP = OCRA
	TCCR0A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00);
	TCCR2A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00);

	OCR0A = 0;
	OCR0B = 0;
	OCR2B = 0;

	// Prescaler 1024
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TCCR2B |= (1 << CS02) | (1 << CS00);

	// PD6
	TCCR0A |= (1 << COM0B1);
	TCCR0A |= (1 << COM0A1);
	TCCR2A |= (1 << COM2B1);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	OCR2B = b;
	OCR0B = g;
	OCR0A = r;
}

void wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		set_rgb(255 - pos * 3, 0, pos * 3);
	} else if (pos < 170) {
		pos = pos - 85;
		set_rgb(0, pos * 3, 255 - pos * 3);
	} else {
		pos -= 170;
		set_rgb(pos * 3, 255 - pos * 3, 0);
	}
}

int main() {

	init_rgb();

	
	
	uint8_t k = 0;
	while (1) {
		wheel(k++);
		_delay_ms(10);
	}
}
