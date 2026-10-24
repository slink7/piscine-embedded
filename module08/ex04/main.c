#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "spi.h"
#include "utils.h"
#include "uart.h"

#define BASE_COLOR 0xE1000000

#define LED_COUNT 3
#define D6 0
#define D7 1
#define D8 2

uint32_t D6_8[3] = {
	BASE_COLOR,
	BASE_COLOR,
	BASE_COLOR
};

uint8_t is_full_rainbow = 0;

uint32_t color(uint8_t r, uint8_t g, uint8_t b) {
	return (BASE_COLOR | ((uint32_t)b << 16) | ((uint32_t)g << 8) | r);
}

uint32_t wheel(uint8_t pos) {
	pos = 255 - pos;
	if (pos < 85) {
		return (color(255 - pos * 3, 0, pos * 3));
	} else if (pos < 170) {
		pos = pos - 85;
		return (color(0, pos * 3, 255 - pos * 3));
	} else {
		pos -= 170;
		return (color(pos * 3, 255 - pos * 3, 0));
	}
}

void update_leds() {
	static uint8_t rainbow_index = 0;

	if (is_full_rainbow) {
		sk9822_start();
		sk9822_set_led(wheel(rainbow_index + 00));
		sk9822_set_led(wheel(rainbow_index + 40));
		sk9822_set_led(wheel(rainbow_index + 80));
		sk9822_stop();
		rainbow_index++;
	} else {
		sk9822_set_leds(D6_8, 3);
	}
}

void tcc1_init() {
	//Time/Counter1 Control Register (A & B)
	//Mettre le timer en mode CTC (WGM12)
	//CTC: Reintialise auto le comteur (TCNT1) quand il depasse le seuil (OCR1A)
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	//Mettre le prescaler a 1024 (CS10)
	TCCR1B |= (1 << CS12) | (1 << CS10);

	//OCR1A = periode du comteur ou declancher un interrupt
	OCR1A = F_CPU / 1024 / 50;

	//Mettre en mode Toggle on Compare Match (pour PB1)
	// TCCR1A |= (1 << COM1A0);
	TIMSK1 |= (1 << OCIE1A);

	// DDRB |= (1 << PB1);
}

ISR(TIMER1_COMPA_vect) {
	update_leds();
}

#define ILEN 11

uint8_t get_hex(char c) {
	return (
		(c >= '0' && c <= '9')
		? c - '0'
		: (c >= 'a' && c <= 'f')
		? c - 'a' + 10
		: (c >= 'A' && c <= 'F')
		? c - 'A' + 10
		: 0xFF
	);
}

uint32_t read_color(char *str) {
	uint32_t out = 0;
	for (uint8_t k = 0; k < 6; ++k) {
		uint8_t tmp = get_hex(str[k]);
		if (tmp & 0xF0)
			return (0xFF000000);
		out |= ((uint32_t)tmp) << (4 * (5 - k));
	}
	return (out);
}

void handle_input(char input[ILEN + 1]) {
	if (!ft_strncmp(input, "FULLRAINBOW", 11)) {
		is_full_rainbow = 1;
		return ;
	}
	uint32_t color = read_color(input);
	if (color & 0xFF000000) {
		uart_printstr(input);
		uart_printstr("\r\nSyntax error: invalid char in color\r\n");
		return ;
	}
	color |= BASE_COLOR;
	if (input[6] != 'D') {
		uart_printstr("Syntax error: Missing 'D'\r\n");
		return ;
	}
	if (input[7] < '6' || input[7] > '8' || input[8] != '\0') {
		uart_printstr("Syntax error: Invalid led number\r\n");
		return ;
	}
	D6_8[input[7] - '6'] = (color & 0xFF00FF00) | (color & 0x00FF0000) >> 16 | (color & 0x000000FF) << 16;
	is_full_rainbow = 0;
}

int main(void) {

	SREG |= (1 << SREG_I);
	tcc1_init();
	spi_master_init();
	uart_init(UBRR);

	char input[ILEN + 1] = {};
	while ('0') {
		uart_tx('#');
		uart_read(input, 11);
		uart_printstr("\r\n");
		handle_input(input);
	}
}
