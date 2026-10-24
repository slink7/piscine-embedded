#include "i2c.h"

//Waits for TWINT: TWI has finished its last job
void i2c_wait(void)
{
	while (!(TWCR & (1 << TWINT)))
		;
}

//Send START: mandatory begin signal for any message
void i2c_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	i2c_wait();
}

//Sends STOP: Mandatory end signal for any message
void i2c_stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while (TWCR & (1 << TWSTO))
		;
}

void i2c_init(void) {
	TWCR = 0;
	TWBR = 72;
	TWSR = 0x00;
	TWCR = (1 << TWEN);
}

void i2c_write(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	i2c_wait();
}

uint8_t i2c_read(uint8_t ack)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (ack << TWEA);
	i2c_wait();
	return (TWDR);
}
