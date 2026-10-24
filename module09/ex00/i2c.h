#ifndef I2C_H
#define I2C_H

#include <avr/io.h>

void i2c_wait(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_init(void);
void i2c_write(unsigned char data);
uint8_t i2c_read(uint8_t ack);

#endif
