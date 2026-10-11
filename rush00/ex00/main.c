
// PC5 SCL
// (2 wire Serial bus clock Line)
// 
//
// TWCR |= (1 << TWEN)
// enables 2 wire serial interface
//
// TWSR prescaler
// TWBR SCL period
//
// PC4 SDA
// (2 wire Serial Data Input/Output Line)
//
// TWAR (2 wire address register)
//
//
// 22.5.2
// SCL_freq = (F_CPU) / (16 + 2 * TWBR * Prescaler)
//
// TWI Interrupt Enable
// TWCR |= (1 << TWIE)
//

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdint.h>



#define S(bit) (1 << bit)
//
//
// #define SET_R(reg, on, off)\
// 	reg = (reg & ~off) | on
//
//
#define WAIT_FOR_TRANSMISSION()\
	while(!(TWCR & S(TWINT)));
//
//
// void send_start() {
// 	//Sends START
// 	SET_R(TWCR,
// 		S(TWINT) | S(TWSTA) | S(TWEN),
// 		S(TWSTO) | S(1)
// 	);
//
// 	WAIT_FOR_TRANSMISSION();
//
// }
//
// //Address packet: 9bits (7bits address, 1bit R/W, 1bit ACK)
//
// void send_byte(uint8_t byte) {
// 	TWDR = 0b00000010;
//
// 	SET_R(TWCR,
// 		S(TWINT) | S(TWEN),
// 		S(TWSTA) | S(TWSTO) | S(1)
// 	);
//
// 	WAIT_FOR_TRANSMISSION();
//
// 	//TWINT (int TWCR) must be high
// 	TWDR = byte;
//
// 	SET_R(TWCR,
// 		S(TWINT) | S(TWEN),
// 		S(TWSTA) | S(TWSTO) | S(1)
// 	);
//
// 	WAIT_FOR_TRANSMISSION();
// }
//
// void send_stop() {
// 	//Sends STOP
// 	SET_R(TWCR,
// 		S(TWINT) | S(TWEN) | S(TWSTO),
// 		S(TWSTA) | S(1)
// 	);
// }
//
// uint8_t is_slave = 0;
//
// void init_slave() {
// 	is_slave = 1;
//
// 	//Sets the prescaler to 1 (driven by master)
// 	TWSR = 0;
//
// 	// For the slave
// 	// Respond to general call
// 	// 7bit addr, 1bit general call
// 	TWAR = 0b00000010;
//
// 	TWCR = S(TWEN) | S(TWEA) | S(TWIE);
//
// 	return ;
//
//
// 	// Allows interrupts for TWI
// 	// TWCR |= (1 << TWIE);
//
// 	// Enable global interrupt
// 	// SREG |= (1 << SREG_I);
//
// 	while (1) {
// 		SET_R(TWCR,
// 			S(TWEA) | S(TWEN),
// 			S(TWINT) | S(TWSTA) | S(TWSTO) | S(TWWC) | S(1)
// 		);
//
// 		WAIT_FOR_TRANSMISSION();
//
// 		uint8_t t = TWDR;
// 		if (t == 'e') {
// 			PINB = S(PINB1);
// 		} else {
// 			PINB = S(PINB2);
// 		}
// 		_delay_ms(100);
// 	}
// }
//
// void init_master() {
//
// 	TWBR = 72;
// 	TWSR = 0x00;
// 	TWCR = (1 << TWEN);
//
// 	send_start();
// 	send_byte('e');
// 	send_stop();
// }
//

uint8_t is_slave = 0;

void twi_init_master(void) {
    TWBR = 72;      // Bit rate register
    TWSR = 0x00;    // Prescaler = 1
    TWCR = (1 << TWEN); // Enable TWI
}

void twi_init_slave(void) {
	TWAR = 0x20;
	TWCR = S(TWEN) | S(TWEA) | S(TWIE);
}

void twi_write_byte(uint8_t addr, uint8_t byte) {
	TWCR = S(TWINT) | S(TWSTA) | S(TWEN);
	WAIT_FOR_TRANSMISSION();

	TWDR = addr;
	TWCR = S(TWINT) | S(TWEN);
	WAIT_FOR_TRANSMISSION();

	TWDR = byte;
	TWCR = S(TWINT) | S(TWEN);
	WAIT_FOR_TRANSMISSION();

	TWCR = S(TWINT) | S(TWSTO) | S(TWEN);
}

int main() {
	// Allows interrupts for TWI
	// TWCR |= (1 << TWIE);

	// Enable global interrupt
	// SREG |= (1 << SREG_I);


	//Sends START
	// send_start();
	// TWSR is set to 0x08

	DDRD = 0;

	while (1) {
		if (!(PIND & S(PD2))) {
			DDRB |= S(DDB0);
			DDRB |= S(DDB2);
			PORTB |= S(PB0);
			_delay_ms(50.0);
			is_slave = 1;
			twi_init_slave();
			break ;
		} else if (!(PIND & S(PD4))) {
			DDRB |= S(DDB4);
			PORTB |= S(PB4);
			_delay_ms(50.0);
			twi_init_master();
			break ;
		}
	}

	SREG |= (1 << SREG_I);

	DDRB = 0x17;

	while (1) {
		if (!is_slave && !(PIND & S(PD2)))
			twi_write_byte(0x20, 0b00001111);
	}

}

ISR(TWI_vect) {
	uint8_t status = TWSR & 0xF8;
	if (status == 0x60) {
		uint8_t byte = TWDR;
		// PORTB = byte;
		PORTB ^= S(PB1);
	} else if (status == 0xA8) {
		TWDR = 0x01;
		TWCR = S(TWINT) | S(TWEA) | S(TWEN);
	}
}
