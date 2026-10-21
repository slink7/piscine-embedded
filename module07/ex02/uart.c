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

void uart_tx(char data) {
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

char uart_rx(void) {
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

void uart_print_u32(uint32_t u32) {
	for (uint8_t k = 0; k < 8; ++k)
		uart_tx(get_nibble(u32 >> (4 * (7 - k))));
}

void uart_print_u16(uint16_t u16) {
	for (uint8_t k = 0; k < 4; ++k)
		uart_tx(get_nibble(u16 >> (4 * (3 - k))));
}

void uart_print_i16(int16_t i16) {
	if (i16 < 0)
		uart_tx('-');
	uart_print_u16((uint16_t) -i16);
}

void uart_print_uhex(uintmax_t umax, uint8_t len) {
	for (uint8_t k = 0; k < len;++k) {
		uart_tx(get_nibble(umax >> (4 * (len - 1 - k))));
	}
}

void uart_print_ihex(intmax_t imax, uint8_t len) {
	if (imax < 0)
		uart_tx('-');
	uart_print_uhex((imax >= 0) ? imax : -imax, len);
}

void uart_print_udec(intmax_t umax) {
	if (umax / 10 > 0)
		uart_print_udec(umax / 10);
	uart_tx('0' + (umax % 10));
}

void uart_print_idec(intmax_t imax) {
	if (imax < 0)
		uart_tx('-');
	uart_print_udec((imax >= 0) ? imax : -imax);
}

void uart_print_pchar(char c) {
	uart_tx((c >= 32 && c < 127) ? c : '.');
}

void uart_read(char *buffer, uint8_t bsize) {
	uint8_t at = 0;
	char in = 0;
	do {
		in = uart_rx();
		if (in == '\e') {
			uart_flush();
			continue ;
		} else if (in == '\r') {
			break ;
		} else if (at > 0 && in == '\x7F') {
			uart_printstr("\e[1D \e[1D");
			buffer[at--] = 0;
			continue ;
		} else if (at >= bsize || in == '\x7F')
			continue ;
		uart_tx(in);
		buffer[at++] = in;
	} while (1);
	buffer[at] = 0;
}

