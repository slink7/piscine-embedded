
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

#define BAUD_PRESCALLER (F_CPU / (UART_BAUDRATE * 16UL))

//UMSEL01 UMSEL00 UPM01 UPM00 USBS0 UCSZ01/UDORD0 UCSZ00/UCPHA0 UCPOL0

void uart_init(unsigned int ubrr) {
	//Sets the baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	//Enable reveiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	//Sets the frame format
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
}

void uart_tx(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

unsigned int uart_rx(void) {
	//Wait for 1 byte to be readable (could be removed because of interrupt)
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}

void uart_printstr(char *s) {
	while (*s) {
		uart_tx(*s);
		s++;
	}
}

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
	OCR0B = r;
	OCR0A = g;
}

void USART_flush() {
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

void get_input(char *buff, uint8_t bsize) {
	unsigned char in;
	uint8_t at = 0;
	do {
		in = uart_rx();
		if (in == '\e') {
			_delay_ms(50.0);
			USART_flush();	
			continue ;
		}
		if (in == '\r') {
			PINB = (1 << PINB1);
			break ;
		}
		if (at > 0 && in == '\x7F') {
			uart_printstr("\e[1D \e[1D");
			buff[at--] = 0;
			continue ;
		}
		if (at >= bsize || in == '\x7F')
			continue ;
		uart_tx(in);
		buff[at++] = in;
	} while (1);
	buff[at] = 0;
}

int8_t get_hex(char c) {
	return (
		(c >= '0' && c <= '9')
		? c - '0'
		: (c >= 'A' && c <= 'F')
		? c - 'A'
		: -1
	);
}

void set_color(char buffer[8]) {
	if (buffer[0] != '#') {
		uart_printstr("Missing '#' prefix\n\r");
		return ;
	}
	int8_t tmp;
	uint8_t byte;
	for (int k = 0; k < 3; ++k) {
		tmp = get_hex(buffer[1 + 2 * k + 0]);
		if (tmp < 0) {
			uart_printstr("Invalid char '");
			buffer[1 + 2 * k + 1] = 0;
			uart_printstr(buffer + 1 + 2 * k + 0);
			uart_printstr("'\r\n");
			return ;
		}
		byte = tmp << 4;
		tmp = get_hex(buffer[1 + 2 * k + 1]);
		if (tmp < 0) {
			uart_printstr("Invalid char '");
			buffer[1 + 2 * k + 2] = 0;
			uart_printstr(buffer + 1 + 2 * k + 1);
			uart_printstr("'\r\n");
			return ;
		}
		byte |= tmp;
		switch (k) {
			case 0: OCR0B = byte; break;
			case 1: OCR0A = byte; break;
			case 2: OCR2B = byte; break;
		}
	}
}

int main() {

	uart_init(BAUD_PRESCALLER);
	init_rgb();
	char buffer[8] = {0};
	while (1) {
		uart_printstr("Color: ");
		get_input(buffer, 7);
		uart_printstr("\r\n");
		set_color(buffer);
	}
}
