
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

void uart_printstr(char *s) {
	while (*s) {
		uart_tx(*s);
		s++;
	}
}

int main() {
	uart_init(BAUD_PRESCALLER);

	//Sets timer mode to PWM
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	//Prescaler 1024
	TCCR1B |= (1 << CS12) | (1 << CS10);

	//Frequency 0.5Hz, period 2s
	OCR1A = F_CPU / 512;

	//Set interrupt on timer compare (OCR1A)
	TIMSK1 |= (1 << OCIE1A);

	//Allows for global interrupt to interrupt the programme
	sei();

	while (1) {
	}
}

//What function to call when the interruption occurs:
//TCNT1 == OCR1A
ISR(TIMER1_COMPA_vect) {
	uart_printstr("Hello World!\n\r");
}
