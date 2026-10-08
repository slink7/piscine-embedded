
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

#define BAUD_PRESCALLER (F_CPU + UART_BAUDRATE * 8UL) / (16UL * UART_BAUDRATE) - 1

#define BUFFER_SIZE 16

#define USER "user"
#define PASS "pass"

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

void USART_flush() {
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

void get_input(char *buff, uint8_t bsize, uint8_t obf) {
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
		uart_tx(obf ? '*' : in);
		buff[at++] = in;
	} while (1);
	buff[at] = 0;
}

int ft_strcmp(char *a, char *b) {
	while (*a && *b && *a == *b) {
		a++;
		b++;
	}
	return (*b - *a);
}

int main() {
	char username[BUFFER_SIZE + 1] = {0};
	char password[BUFFER_SIZE + 1] = {0};

	uart_init(BAUD_PRESCALLER);

	while ("while") {
		uart_printstr("Enter your login:\n\r");
		uart_printstr("\tusername: ");
		get_input(username, BUFFER_SIZE, 0);
		uart_printstr("\r\n\tpassword: ");
		get_input(password, BUFFER_SIZE, 1);
		uart_printstr("\r\n");
		if (ft_strcmp(USER, username) == 0 && ft_strcmp(PASS, password) == 0) {
			uart_printstr("Hello ");
			uart_printstr(username);
			uart_printstr("\n\r\e[33mLet's disco dance !\e[0m\n\r");
			for (int k = 0; k < 120; k++) {
				PINB = (!!(k % 2) << PINB0);
				PINB = (!!(k % 3) << PINB1);
				PINB = (!!(k % 4) << PINB2);
				PINB = (!!(k % 5) << PINB4);
				_delay_ms(200);
			}
			break ;
		} else {
			uart_printstr("Bad combinaison username/password\n\n\r");
		}
	}
}
