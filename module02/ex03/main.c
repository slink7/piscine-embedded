
#include <avr/io.h>
#include <avr/interrupt.h>

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
	UCSR0B |= (1 << RXCIE0);

	//Sets the frame format
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
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

ISR(USART_RX_vect) {
	unsigned char c = uart_rx();
	c += ((c >= 'A' && c <= 'Z') - (c >= 'a' && c <= 'z')) * ('a' - 'A');
	uart_tx(c);
}
