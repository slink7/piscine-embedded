
#include <avr/io.h>
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

void uart_printstr(char *s) {
	while (*s) {
		uart_tx(*s);
		s++;
	}
}

void uart_print_bar(uint8_t byte, uint8_t max) {
	uart_tx(' ');
	uart_tx('|');
	for (uint8_t k = 0; k < max; ++k) {
		uart_tx((k <= (max * byte / 0xFF) ? '=' : ' '));
	}
	uart_tx('|');
}

char get_nibble(uint8_t n) {
	n &= 0xF;
	return (
		(n > 9)
		? 'A' + n - 10
		: '0' + n
	);
}

void write_hex_byte(char buf[2], uint8_t v) {
	buf[1] = get_nibble(v >> 0);
	buf[0] = get_nibble(v >> 4);
}

#define ADCM	0b00000011
#define RV1		0b00000000
#define LDR		0b00000001
#define NTC		0b00000010

void ADC_set_in(uint8_t in) {
	ADMUX = ADMUX & ~ADCM | (ADCM & in);
}

void ADC_init() {
	DDRC &= ~(1 << DDC0);

	// Disabling power reduction for ADC
	// PRR &= ~(1 << PRADC);
	
	//PC0 ADC0 (0b0000) ADC 
	//PC1 ADC1 (0b0001) LDR light
	//PC2 ADC2 (0b0010) NTC temperature

	// ADC Multiplexer Selection Register
	ADMUX = ADMUX
	// Set AVCC voltage reference
		| (1 << REFS0)
	// Left adjust the result
		| (1 << ADLAR)
	;

	// ADC Control and Status Register A 
	ADCSRA = ADCSRA 
	// ADC Enable
		| (1 << ADEN)
	// Sets the ADC Prescaler to 128
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	;
}

void ADC_read(uint8_t *out) {
	
	// Start conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to end
	while (ADCSRA & (1 << ADSC));

	*out = ADCH;
}

int main() {

	uart_init(UBRR);
	
	ADC_init();

	char buff[13] = {0};
	buff[2] = ',';
	buff[3] = ' ';
	buff[6] = ',';
	buff[7] = ' ';
	buff[10] = '\r';
	buff[11] = '\n';

	while (1) {

		uint16_t rv1 = 0;
		uint16_t ldr = 0;
		uint16_t ntc = 0;

		ADC_set_in(RV1);
		ADC_read(&rv1);

		ADC_set_in(LDR);
		ADC_read(&ldr);

		ADC_set_in(NTC);
		ADC_read(&ntc);

		write_hex_byte(buff + 0, rv1);
		write_hex_byte(buff + 4, ldr);
		write_hex_byte(buff + 8, ntc);
		uart_printstr(buff);

		_delay_ms(20);
	}

}
