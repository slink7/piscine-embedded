
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

		write_hex_byte(buff, i);
		uart_printstr(buff);

		_delay_ms(20);
	}
}

//During 1min run
//printed 2981 times
// = 49 print/sec
// ~= 20ms
