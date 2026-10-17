
#include <avr/io.h>
#include <stdlib.h>
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

uint8_t i2c_read(uint8_t ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);
	i2c_wait();
	return (TWDR);
}

uint8_t aht_status() {
	uint8_t out;

	i2c_start();
	i2c_write(0x38 << 1 | 0);
	i2c_write(0x71);
	i2c_stop();

	i2c_start();
	i2c_write(0x38 << 1 | 1);
	out = i2c_read(0);
	i2c_stop();
	
	return (out);
}

void aht_trigger() {
	i2c_start();
	i2c_write(0x38 << 1 | 0);
	i2c_write(0xAC);
	i2c_write(0x33);
	i2c_write(0x00);
	i2c_stop();
}

void aht_read(uint8_t buff[7]) {
	i2c_start();
	i2c_write(0x38 << 1 | 1);
	for (int k = 0; k < 7; ++k)
		buff[k] = i2c_read(k < 6);
	i2c_stop();
}

void aht_init() {
	i2c_start();
	i2c_write(0x38 << 1 | 0);
	i2c_write(0xBE);
	i2c_write(0x08);
	i2c_write(0x00);
	i2c_stop();
	_delay_ms(10.0);
}

uint8_t crc8(const uint8_t *data, uint8_t len) {
	uint8_t crc = 0xFF;

	for (uint8_t k = 0; k < len; ++k) {
		crc ^= data[k];
		for (uint8_t l = 0; l < 8; ++l) {
			if (crc & 0x80) {
				crc = ((crc << 1) ^ 0x31);
			} else {
				crc = (crc << 1);
			}
		}
	}
	return (crc);
}

uint32_t get_bits(uint8_t *data, uint8_t boff, uint8_t blen) {
	uint32_t raw_out = 0;

	for (uint8_t k = 0; k < blen; ++k) {
		uint8_t bit_pos = boff + k;
		uint8_t byte_index = bit_pos / 8;
		uint8_t bit_index = 7 - (bit_pos % 8);
		raw_out = (raw_out << 1) | ((data[byte_index] >> bit_index) & 1u);
	}

	return (raw_out);
}

double get_temperature(uint8_t *data) {
	return (200.0 * (double)get_bits(data, 28, 20) / 1048576.0 - 50.0);
}

double get_humidity(uint8_t *data) {
	return (100.0 * (double)get_bits(data, 8, 20) / 1048576.0);
}

double mean(double *arr, uint8_t len) {
	double sum = 0.0;
	for (uint8_t k = 0; k < len; ++k)
		sum += arr[k];
	return (sum / (double)len);
}

int main() {
	uint8_t raw[7] = {};
	uint8_t crc = 0;
	double temperatures[3] = {};
	double humidities[3] = {};

	uart_init(UBRR);
	i2c_init();
	_delay_ms(100.0);

	uint8_t status = aht_status();

	print_hex_value(status);

	if (!(status & (1 << 3))) {
		uart_printstr("Initilizing...\n\r");
		aht_init();
	}

	uint8_t counter = 0;

	while (1) {
		_delay_ms(10.0);
		aht_trigger();
		_delay_ms(80.0);
		aht_read(raw);

		crc = crc8(raw, 6);
		if (crc != raw[6]) {
			uart_printstr("Invalid crc\r\n");
			print_hex_value(crc);
			uart_printstr(" ");
			print_hex_value(raw[6]);
			uart_printstr("\r\n\n");
			continue;
		}

		// Read humidity
		for (uint8_t k = 0; k < 2; ++k)
			humidities[k] = humidities[k + 1];
		humidities[2] = get_humidity(raw);

		// Read temperatures
		for (uint8_t k = 0; k < 2; ++k)
			temperatures[k] = temperatures[k + 1];
		temperatures[2] = get_temperature(raw);

		// continue if not enough samples to mean
		if (counter++ < 2)
			continue ;

		// Display the means
		char buff[64] = {};
		dtostrf(mean(humidities, 3), 0x3, 0x3, buff);
		uart_printstr(buff);
		uart_printstr("%, ");
		dtostrf(mean(temperatures, 3), 0x3, 0x2, buff);
		uart_printstr(buff);
		uart_printstr("C, \r\n");

		//0b00011001


		// _delay_ms(1000.0);
	}
}
