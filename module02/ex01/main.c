
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

	//Sets the frame format
	UCSR0C = (1 << USBS0) | (3 << UCSZ00);
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

int main() {
	uart_init(BAUD_PRESCALLER);

	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	TCCR1B |= (1 << CS12) | (1 << CS10);

	OCR1A = F_CPU / 512;

	TIMSK1 |= (1 << OCIE1A);

	sei();

	while (1) {
	}
}

ISR(TIMER1_COMPA_vect) {
	uart_printstr("Hello World!\n");
}
