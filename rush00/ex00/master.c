#include "rush.h"

uint8_t	twi_last_status = 0;

//Waits for TWINT: TWI has finished its last job
static uint8_t twi_wait(void)
{
	uint16_t count = 0;

	while (!(TWCR & (1 << TWINT)))
	{
		if (count > 5000)
			return (0);
		count++;
	}
	return (1);
}

//Send START: mandatory begin signal for any message
uint8_t twi_master_start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	if (!twi_wait())
	{
		twi_last_status = 0xFF;
		return (0);
	}
	twi_last_status = TW_STATUS;
	return (twi_last_status == TW_START || twi_last_status == TW_REP_START);
}

//Sends arbitrary byte on the TWI, waits for a rep, compares to expected
uint8_t twi_master_write(uint8_t data, uint8_t expected)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	if (!twi_wait())
	{
		twi_last_status = 0xFF;
		return (0);
	}
	twi_last_status = TW_STATUS;
	return (twi_last_status == expected);
}

//Sends STOP: Mandatory end signal for any message
void twi_master_stop(void)
{
	uint16_t	timeout = 0;

	TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
	while ((TWCR & (1 << TWSTO)) && ++timeout < 5000)
		;
}

//Waits for response, then reads last information byte (NACK)
//We only send 1 byte for button press, so ACK support is not needed
uint8_t twi_read_nack(uint8_t *data)
{
	TWCR = (1 << TWINT) | (1 << TWEN);	// pas de TWEA : NACK apres l'octet
	if (!twi_wait())
	{
		twi_last_status = 0xFF;
		return (0);
	}
	twi_last_status = TW_STATUS;
	if (twi_last_status != TW_MR_DATA_NACK)
		return (0);
	*data = TWDR;
	return (1);
}

//Sends complete message to get pressed from SLAVE
//START - SLA+R - *READ* - STOP
uint8_t	twi_read_press(uint8_t *pressed)
{
	uint8_t	ok;

	ok = twi_master_start()
		&& twi_master_write((SLAVE_ADDR << 1) | TW_READ, TW_MR_SLA_ACK)
		&& twi_read_nack(pressed);
	twi_master_stop();	// toujours, meme en cas d'echec
	return (ok);
}

//Sends complete message to give SLAVE the game state (leds)
//START - SLA+W - LEDS (D1-D4) - RGB (D5) - STOP
uint8_t	twi_send_state(uint8_t leds, uint8_t w)
{
	uint8_t ok;

	ok = twi_master_start()
		&& twi_master_write((SLAVE_ADDR << 1) | TW_WRITE, TW_MT_SLA_ACK)
		&& twi_master_write(leds, TW_MT_DATA_ACK)
		&& twi_master_write(w, TW_MT_DATA_ACK);
	twi_master_stop();	// toujours, meme en cas d'echec
	return (ok);
}

//poll
// Main networking function
// Asks for SLAVE's button press,
// and updates it on the game state
uint8_t	master_poll(uint8_t *slave_pressed)
{
	uint8_t	read_status;
	uint8_t	send_status;
	uint8_t	pressed = 0;
	uint8_t	leds;

	//Read the MASTER's leds
	leds = PORTB & LEDS_MASK;
	if (is_slave_ready && !game_started)
		leds |= READY_BIT;
	//Send the game info
	send_status = twi_send_state(leds, winner);

	//Wait a while for TWI to send another message
	twi_wait();
	// _delay_us(100);

	// Fetch the SLAVE's button press
	read_status = twi_read_press(&pressed);
	*slave_pressed = (read_status && pressed);

	// If any message went wrong, we reset
	if (!send_status || !read_status)
		twi_init_master();

	return (read_status && send_status);
}
