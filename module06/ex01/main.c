
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

void print_hex_value(char v) {
	uart_tx(get_nibble(v >> 4));
	uart_tx(get_nibble(v >> 0));
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
}

//Sends STOP: Mandatory end signal for any message
void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO))
		;
}

void i2c_init(void) {
	TWCR = 0;
	TWBR = 72;
	TWSR = 0x00;
	TWCR = (1 << TWEN);
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait();
}

void i2c_read(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait();
	print_hex_value(TWDR);
}

// void aht_status() {
// 	i2c_start();
// 	i2c_write(0x38 << 1 | 1);
// 	i2c_write(0x71);
// 	i2c_read();
// 	i2c_stop();
// }

void aht_trigger() {
	i2c_start();
	i2c_write(0x38 << 1 | 0);
	i2c_write(0xAC);
	i2c_write(0x33);
	i2c_write(0x00);
	i2c_stop();
}

void aht_read() {
	i2c_start();
	i2c_write(0x38 << 1 | 1);
	for (int k = 0; k < 7; ++k) {
		i2c_read();
		uart_printstr((k == 6) ? "\r\n" : " ");
	}
	i2c_stop();
}

int main() {
	uart_init(UBRR);
	i2c_init();
	_delay_ms(100.0);

	while (1) {
		_delay_ms(10.0);
		aht_trigger();
		_delay_ms(80.0);
		aht_read();
	}
}
