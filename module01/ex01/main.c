#include <avr/io.h>
// #include <avr/interrupt.h>

#ifndef F_CPU
	#warning "Missing -DF_CPU compilation flags"
	#define F_CPU 16000000UL
#endif

int main() {

	DDRB |= (1 << PB1);

	//Time/Counter1 Control Register (A & B)
	//Mettre le timer en mode CTC (WGM12)
	//CTC: Reintialise auto le comteur (TCNT1) quand il depasse le seuil (OCR1A)
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	//Mettre le prescaler a 1024 (CS10)
	TCCR1B |= (1 << CS12) | (1 << CS10);

	//OCR1A = periode du comteur ou declancher un interrupt
	OCR1A = F_CPU / 2048;

	//Mettre en mode Toggle on Compare Match (pour PB1)
	TCCR1A |= (1 << COM1A0);

	while (1) {
	}
}
