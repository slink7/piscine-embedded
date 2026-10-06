
#include <avr/io.h>
#include <util/delay.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

#define BAUD_PRESCALLER (((unsigned int)(F_CPU / (UART_BAUDRATE * 16UL))))

//UMSEL01 UMSEL00 UPM01 UPM00 USBS0 UCSZ01/UDORD0 UCSZ00/UCPHA0 UCPOL0

void USART_init(unsigned int ubrr) {
	//Sets the baud rate
	UBRR0H = (unsigned char) (ubrr >> 8);
	UBRR0L = (unsigned char) ubrr;

	//Enable reveiver and transmitter
	UCSR0B = (1 << RXEN0) | (1 << TXEN0);

	//Sets the frame format
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
	// UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void USART_transmit(unsigned char data) {
	while (!(UCSR0A & (1 << UDRE0)))
		;
	UDR0 = data;
}

unsigned int USART_receive(void) {
	unsigned char status, resh, resl;

	while (!(UCSR0A & (1 << RXC0)))
		;

	status = UCSR0A;
	resh = UCSR0B;
	resl = UDR0;

	if (status & (1 << FE0) | (1 << DOR0) | (1 << UPE0))
		return (-1);

	resh = (resh >> 1) & 0x01;
	return ((resh << 8) | resl);
}

void send(char *s) {
	while (*s) {
		USART_transmit(*s);
		s++;
	}
}

void USART_flush() {
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

int main() {
	USART_init(BAUD_PRESCALLER);
	USART_flush();
	send("0123456789ABCDEF");
	char c = '0';
	while (1) {
		USART_transmit(c);
		_delay_ms(10);
	}
}
