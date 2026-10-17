#include <avr/io.h>
#include <stdint.h>

void uart_init(uint16_t ubrr) {
	//Sets the baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	//Enable the receiver and the transmitter
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

uint8_t uart_rx(void) {
	while (!(UCSR0A & (1 << RXC0)))
		;
	return UDR0;
}

void uart_flush(void) {
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
	(void) dummy;
}

static char get_nibble(uint8_t n) {
	n &= 0xF;
	return (
		(n > 9)
		? 'A' + n - 10
		: '0' + n
	);
}

void uart_print_byte(uint8_t byte) {
	uart_tx(get_nibble(byte >> 4));
	uart_tx(get_nibble(byte >> 0));
}

void uart_print_ui32(uint32_t ui32) {
	for (uint8_t k = 0; k < 8; ++k)
		uart_tx(get_nibble(ui32 >> (4 * (7 - k))));
}

void uart_print_pchar(char c) {
	uart_tx((c >= 32 && c < 127) ? c : '.');
}
