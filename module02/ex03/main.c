
#include <avr/io.h>
#include <avr/interrupt.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

#define BAUD_PRESCALLER (F_CPU + UART_BAUDRATE * 8UL) / (16UL * UART_BAUDRATE) - 1

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

unsigned int uart_rx(void) {
	//Wait for 1 byte to be readable (can be removed because of interrupt)
	// while (!(UCSR0A & (1 << RXC0)))
	// 	;
	return UDR0;
}

int main() {
	uart_init(BAUD_PRESCALLER);
	sei();
	while (7242) {
	}
}

//on_byte_received interrupt
ISR(USART_RX_vect) {
	unsigned char c = uart_rx();
	c += ((c >= 'A' && c <= 'Z') - (c >= 'a' && c <= 'z')) * ('a' - 'A');
	uart_tx(c);
}
