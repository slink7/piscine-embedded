
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

#define UBRR (F_CPU + UART_BAUDRATE * 8UL) / (16UL * UART_BAUDRATE) - 1

void init_rgb() {
	DDRD |= (1 << DDD3) | (1 << DDD5) | (1 << DDD6);

	//Timer, mode 7. FastPWM, TOP = OCRA/B
	TCCR0A |= (1 << WGM02) | (1 << WGM01) | (1 << WGM00);
	TCCR2A |= (1 << WGM22) | (1 << WGM21) | (1 << WGM20);

	OCR0A = 0;
	OCR0B = 0;
	OCR2B = 0;

	// Prescaler 1024
	TCCR0B |= (1 << CS02) | (1 << CS00);
	TCCR2B |= (1 << CS22) | (1 << CS20);

	// PD5
	TCCR0A |= (1 << COM0B1);
	// PD6
	TCCR0A |= (1 << COM0A1);
	// PD3
	TCCR2A |= (1 << COM2B1);
}

void set_rgb(uint8_t r, uint8_t g, uint8_t b) {
	// PD3
	OCR2B = b;
	// PD5
	OCR0B = r;
	// PD6
	OCR0A = g;
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

void uart_init(unsigned int ubrr) {
	//Sets the baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	//Enable reveiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	//Sets the frame format: 8N1 (Disabled parity & 1 stop bit are defaults)
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_tx(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

void uart_printstr(char *s) {
	while (*s) {
		uart_tx(*s);
		s++;
	}
}

void uart_print_bar(uint8_t byte) {
	uart_tx(' ');
	uart_tx('|');
	for (uint8_t k = 0; k < 16; ++k) {
		uart_tx((k <= (byte >> 4) ? '=' : ' '));
	}
	uart_tx('|');
}

char get_nibble(uint8_t n) {
	n &= 0xF;
	return (
		(n > 9)
		? 'A' + n - 10
		: '0' + n
	);
}

void write_hex_byte(char buf[2], uint8_t v) {
	buf[1] = get_nibble(v >> 0);
	buf[0] = get_nibble(v >> 4);
}

void RV1_init() {
	DDRC &= ~(1 << DDC0);

	// Disabling power reduction for ADC
	PRR &= ~(1 << PRADC);
	
	// ADC Multiplexer Selection Register
	ADMUX = ADMUX
	// Set AVCC voltage reference
		| (1 << REFS0)
	// Left adjust the result
		| (1 << ADLAR)
	;

	// ADC Control and Status Register A 
	ADCSRA = ADCSRA 
	// ADC Enable
		| (1 << ADEN)
	// Sets the ADC Prescaler to 128
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	;
}

int main() {

	uart_init(UBRR);
	init_rgb();
	
	RV1_init();

	char buff[5] = {0};
	buff[2] = '\r';
	buff[3] = '\n';

	while (1) {

		// Start conversion
		ADCSRA |= (1 << ADSC);

		// Wait for the conversion to end
		while (ADCSRA & (1 << ADSC));

		// read ADC value
		uint8_t i = ADCH;

		uart_print_bar(i);
		write_hex_byte(buff, i);
		uart_printstr(buff);

		DDRB = 23;

		wheel(i);

		PORTB = i >> 4;
		PORTB = (PORTB & ~(1 << PORTB4)) | (!!(PORTB & (1 << PORTB3)) << PORTB4);
		_delay_ms(20);
	}

}
