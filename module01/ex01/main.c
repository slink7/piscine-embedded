#include <avr/io.h>
// #include <avr/interrupt.h>

#ifndef F_CPU
	#warning "Missing -DF_CPU compilation flags"
	#define F_CPU 16000000UL
#endif

//Macros directement reprises du interrupt.h
//Set le flag global SEI
# define sei()  __asm__ __volatile__ ("sei" ::: "memory")

//Set une fonction dans la zone .text reseve.
# define ISR(vector, ...)            \
    void vector (void) __attribute__ ((__signal__)) __VA_ARGS__; \
    void vector (void)

int main() {

	//Time/Counter1 Control Register (A & B)
	//Mettre le timer en mode CTC (WGM12)
	//CTC: Reintialise auto le comteur (TCNT1) quand il depasse le seuil (OCR1A)
	TCCR1A = 0;
	TCCR1B = (1 << WGM12);

	//Mettre le prescaler a 1024 (CS10)
	TCCR1B |= (1 << CS12) | (1 << CS10);

	//OCR1A = periode du comteur ou declancher un interrupt
	OCR1A = F_CPU / 2048;

	//Timer/Counter1 Interrupt Mask Register
	//OCIE1A = Output Compare Interrupt Enable 1A
	//Active l'utilisation de OCR1A
	TIMSK1 |= (1 << OCIE1A);

	//Active les interruptions
	//"Set Global Interrupt Enable"
	sei();

	while (1) {
	}
}

ISR(TIMER1_COMPA_vect) {
	PINB = (1 << PINB1);
}
