
#include <avr/io.h>
#include <util/twi.h>
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

void uart_flush() {
	unsigned char dummy;
	while (UCSR0A & (1 << RXC0))
		dummy = UDR0;
}

char get_nibble(uint8_t n) {
	n &= 0xF;
	return (
		(n > 9)
		? 'A' + n - 10
		: '0' + n
	);
}

void uart_hex_byte(uint8_t v) {
	uart_tx(get_nibble(v >> 0));
	uart_tx(get_nibble(v >> 4));
}


//Waits for TWINT: TWI has finished its last job
void i2c_wait(void)
{
	while (!(TWCR & (1 << TWINT)))
		;
}

//Send START: mandatory begin signal for any message
void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	i2c_wait();
	uart_hex_byte(TW_STATUS);
}

//Sends STOP: Mandatory end signal for any message
void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO))
		;
	uart_hex_byte(TW_STATUS);
}

void i2c_init(void) {
	TWCR = 0;
	TWBR = 72;
	TWSR = 0x00;
	TWCR = (1 << TWEN);
}

int main() {
	uart_init(UBRR);
	i2c_init();
	i2c_start();
	uart_printstr(", ");
	i2c_stop();
}
