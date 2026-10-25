#include <avr/io.h>
#include <util/delay.h>

#include "i2c.h"

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

#define INPUT_PORT_0			0x00
#define OUTPUT_PORT_0			0x02
#define CONFIGURATION_PORT_0	0x06

#define BASE_ADDRESS			0b01000000
#define ADDRESS					BASE_ADDRESS | (0b010 << 1)

#define ACK		1
#define NACK	0

#define READ	1
#define WRITE	0

uint8_t get_reg0() {
	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(INPUT_PORT_0);
	i2c_start();
	i2c_write(ADDRESS | READ);
	uint8_t out = i2c_read(ACK);
	i2c_stop();
	return (out);
}

int main(void) {

	i2c_init();

	i2c_start();
	i2c_write(ADDRESS | 0);
	i2c_write(CONFIGURATION_PORT_0); // Command 6: Configuration port 0
	i2c_write(0b11110001); // (pin 1, 2 & 3)
	i2c_stop();

	uint8_t mem = 0;
	uint8_t edge = 0;

	while ('0') {

		// Read register 0 
		uint8_t reg = get_reg0();

		// Debounce && detect falling edge: increment mem
		if (!(reg & 0b00000001)) {
			_delay_ms(10);
			if (!edge && !(get_reg0() & 0b00000001)) {
				mem = (mem + 1) % 8;
				edge = 1;
			}
		} else {
			edge = 0;
		}

		// Send led state
		i2c_start();
		i2c_write(ADDRESS | WRITE);
		i2c_write(OUTPUT_PORT_0); // Command 2 Output port 0
		i2c_write(0b11110001 | ~(mem << 1)); // Display 3bit num to pin 1, 2 & 3
		i2c_stop();

		_delay_ms(10);
	}
}
