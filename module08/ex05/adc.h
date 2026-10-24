#ifndef ADC_H
#define ADC_H

#include <avr/io.h>

#define ADCM	0b00000011
#define RV1		0b00000000
#define LDR		0b00000001
#define NTC		0b00000010

void adc_set_in(uint8_t in);
void adc_init(void);
uint8_t adc_read(void);

#endif
