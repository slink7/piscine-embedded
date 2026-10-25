#include <avr/io.h>
#include <util/delay.h>

#include "i2c.h"

//
// D9
// IO0_3
//


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
#define CONFIGURATION_PORT_0	0x06

#define BASE_ADDRESS			0b01000000
#define ADDRESS					BASE_ADDRESS | (0b010 << 1)

int main(void) {

	i2c_init();

	i2c_start();
	i2c_write(ADDRESS | 0);
	i2c_write(CONFIGURATION_PORT_0); // Command 6: Configuration port 0
	i2c_write(~(1 << 3)); // (pin 3)
	i2c_stop();

	uint8_t mem = 0;

	while ('0') {
		i2c_start();
		i2c_write(ADDRESS | 0);
		i2c_write(OUTPUT_PORT_0); // Command 2 Output port 0
		i2c_write(~(1 << 3) | ((mem) ? 0x08 : 0x00)); // Toggle pin 3 on and off
		i2c_stop();
		mem = !mem;
		_delay_ms(500);
	}
}
