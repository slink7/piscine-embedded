
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

void uart_print_nbr_req(uint16_t n, char *dst) {
	if (n == 0) {
		*dst = '0';
		return ;
	}
	if (n / 10 > 0)
		uart_print_nbr_req(n / 10, dst - 1);
	*dst = (n > 0) ? '0' + n % 10 : ' ';
}

void uart_print_nbr(uint16_t n) {
	char buff[5] = {' ', ' ', ' ', ' '};
	uart_print_nbr_req(n, buff + 3);
	uart_printstr(buff);
}

void ADC_init() {
	DDRC &= ~(1 << DDC0);

	// Disabling power reduction for ADC
	// PRR &= ~(1 << PRADC);
	
	//PC0 ADC0 (0b0000) ADC 
	//PC1 ADC1 (0b0001) LDR light
	//PC2 ADC2 (0b0010) NTC temperature
	//    ADC8 (0b1000) on-chip temperature

	// ADC Multiplexer Selection Register
	ADMUX = ADMUX
	// Set AVCC voltage reference
		| (1 << REFS1) | (1 << REFS0)
	// Left adjust the result
		// | (1 << ADLAR)
	// Set input channel to ADC8 (on-chip temerature)
		| (1 << MUX3)
	;

	// ADC Control and Status Register A 
	ADCSRA = ADCSRA 
	// ADC Enable
		| (1 << ADEN)
	// Sets the ADC Prescaler to 128
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	;
}

void ADC_read(uint16_t *out) {
	
	// Start conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to end
	while (ADCSRA & (1 << ADSC));

	*out = ADC;
}

int main() {

	uart_init(UBRR);
	
	ADC_init();

	while (1) {

		uint16_t tmp = 0;
		ADC_read(&tmp);
		uart_print_nbr((tmp - 324.31) / 1.22);
		uart_printstr("°C\r\n");

		_delay_ms(20);
	}

}
