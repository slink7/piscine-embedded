
#include "adc.h"

void adc_set_in(uint8_t in) {
	ADMUX = (ADMUX & ~ADCM) | (ADCM & in);
}

void adc_init(void) {
	DDRC &= ~(1 << DDC0);

	// Disabling power reduction for ADC
	PRR &= ~(1 << PRADC);
	
	//PC0 ADC0 (0b0000) ADC 
	//PC1 ADC1 (0b0001) LDR light
	//PC2 ADC2 (0b0010) NTC temperature

	// ADC Multiplexer Selection Register
	ADMUX = ADMUX
	// Set AVCC voltage reference
		| (1 << REFS0)
	// Left adjust the result
		// | (1 << ADLAR)
	;

	// ADC Control and Status Register A 
	ADCSRA = ADCSRA 
	// ADC Enable
		| (1 << ADEN)
	// Sets the ADC Prescaler to 128
		| (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0)
	;
}

uint16_t adc_read(void) {
	
	// Start conversion
	ADCSRA |= (1 << ADSC);

	// Wait for the conversion to end
	while (ADCSRA & (1 << ADSC));

	return (ADC);
}

