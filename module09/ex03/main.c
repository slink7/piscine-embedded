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

int main(void) {

	i2c_init();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(CONFIGURATION_PORT_1); // Command 7: Configuration port 1
	i2c_write(0b00000000); // (pin 0..7: 7seg a-g + DP)
	i2c_stop();

	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(CONFIGURATION_PORT_0); // Command 6: Configuration port 0
	i2c_write(0b01111111); // (pin 4..7: CA1..4 (7seg digits))
	i2c_stop();
	


	i2c_start();
	i2c_write(ADDRESS | WRITE);
	i2c_write(OUTPUT_PORT_0); // Command 2 Output port 0
	i2c_write(0b01111111); // Set output to be the rightmost digit
	i2c_stop();

	uint8_t digits[10] = {
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

	uint8_t mem = 0;

	while ('0') {
		i2c_start();
		i2c_write(ADDRESS | WRITE);
		i2c_write(OUTPUT_PORT_1); // Command 3 Output port 1
		i2c_write(digits[mem]); // Use digits[] to display correct pattern
		i2c_stop();

		mem = (mem + 1) % 10;

		_delay_ms(1000);
	}
}
