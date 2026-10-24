#include <avr/common.h>
#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "adc.h"
#include "spi.h"

#define BASE_COLOR 0xE1000000

#define LED_COUNT 3
#define D6 0
#define D7 1
#define D8 2

uint32_t D6_8[3] = {
	BASE_COLOR,
	BASE_COLOR,
	BASE_COLOR
};

volatile uint8_t led = 0;
volatile uint8_t component = 0;

ISR(INT0_vect) {
	_delay_ms(50.0);
	//Certifie par paolo
	if ((PIND & (1 << PIND2)))
		return ;
	component = (component + 1) % 3;
}

ISR(PCINT2_vect) {
	_delay_ms(50.0);
	if ((PIND & (1 << PIND4)))
		return ;
	component = 0;
	led = (led + 1) % 3;
}

void sw_init() {
	//External Interrupt Mask Register
	//Allows the PD2 (SW1) to interrupt (secondary function)
	EIMSK |= (1 << INT0);
	//External Interrupt Control Register A
	// Sets the control to: interrupt on any logical change on INT0
	EICRA |= (1 << ISC00);

	//Pin Change Mask Register
	//Allows PD4 to interrupt (secondary function)
	PCMSK2 |= (1 << PCINT20);
	//Pin Change Interrupt Control Register
	//Allows interrupt on any change on PCINT[23:16]
	PCICR |= (1 << PCIE2);

	//Allows global interrupt (sei())
	SREG |= (1 << SREG_I);
}

int main(void) {

	spi_master_init();
	sw_init();
	adc_init();

	while ('0') {
		uint8_t value = adc_read();
		uint8_t shift = component * 8;
		D6_8[led] = (D6_8[led] & ~((uint32_t)0xFF << shift)) | ((uint32_t)value << shift);
		sk9822_set_leds(D6_8, 3);
	}
}
