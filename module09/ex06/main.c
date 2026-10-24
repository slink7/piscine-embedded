#include <avr/io.h>
#include <stdint.h>
#include <util/delay.h>
#include <avr/interrupt.h>

#include "i2c.h"
#include "adc.h"

#define MY_ISR(vector)                                             \
  void vector(void) __attribute__((__signal__, __INTR_ATTRS));     \
  void vector(void)

//
// D9
// IO0_3
//

// https://www.nxp.com/docs/en/data-sheet/PCA9555.pdf

// Command	Register
// 0		Input port 0
// 1		Input port 1
// 2		Output port 0
// 3		Output port 1
// 4		Polarity Inversion port 0
// 5		Polarity Inversion port 1
// 6		Configuration port 0
// 7		Configuration port 1

#define OUTPUT_PORT_0			0x02
#define OUTPUT_PORT_1			0x03
#define CONFIGURATION_PORT_0	0x06
#define CONFIGURATION_PORT_1	0x07

#define BASE_ADDRESS			0b01000000
#define ADDRESS					BASE_ADDRESS | (0b010 << 1)

#define ACK		1
#define NACK	0

#define READ	1
#define WRITE	0

#define TWO 0b01011011

const uint8_t digits[10] = {
	0b00111111, //0
	0b00000110, //1
	0b01011011, //2
	0b01001111, //3
	0b01100110, //4
	0b01101101, //5
	0b01111101, //6
	0b00000111, //7
	0b01111111, //8
	0b01101111, //9
};

#define DELAY 2

void set_digit(uint8_t pos, uint8_t digit) {
	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(OUTPUT_PORT_0); // Command 2 Output port 0
	i2c_write(0b11111111); // Display 3bit num to pin 1, 2 & 3
	i2c_stop();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(OUTPUT_PORT_1); // Command 3 Output port 1
	i2c_write(digits[digit % 10]); // Toggles a, b, d, e, g to display '2' on 7seg
	i2c_stop();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(OUTPUT_PORT_0); // Command 2 Output port 0
	i2c_write(~(16 << pos));
	i2c_stop();
}

void set_number(uint16_t number) {
	uint8_t digits[4] = {};

	for (uint8_t k = 0; k < 4; ++k) {
		digits[k] = number % 10;
		number /= 10;
	}

	for (uint8_t k = 0; k < 4; ++k) {
		set_digit(k, digits[3 - k]);
		_delay_ms(DELAY);
	}
}


int main(void) {

	adc_init();
	i2c_init();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(CONFIGURATION_PORT_1); // Command 7: Configuration port 1
	i2c_write(0b00000000); // (pin 0..7: 7seg a-g + DP)
	i2c_stop();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(CONFIGURATION_PORT_0); // Command 6: Configuration port 0
	i2c_write(0b00001111); // (pin 4..7: CA1..4 (7seg digits))
	i2c_stop();

	while (1) {
		uint16_t rv1 = adc_read();
		set_number(rv1);
	}
}
