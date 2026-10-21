#include "rush.h"

// __vector_24 = TWI_vect
// The interrupt function called when TWI has received/finished its job
void __vector_24(void)
{
	static uint8_t  index = 0;
	static uint8_t  rx[2];

	switch (TW_STATUS)
	{
		// ========================================
		// READING FROM MASTER (SR, Slave Receive)
		// ========================================

		// MASTER sent 0x60: SLA+W
		case TW_SR_SLA_ACK:
			// Accept our fate: we are SLAVE
			index = 0;
			player = SLAVE;
			// Reset reset cooldown
			slave_idle = 0;
			break;

		// MASTER sent 0x80: DATA
		case TW_SR_DATA_ACK:
			if (index < 2)
				//Read the data
				rx[index++] = TWDR;
			break;

		// MASTER sent 0xA0: STOP  
		case TW_SR_STOP:
			if (index == 2)
			{
				//Sets the lights as the MASTER wishes
				PORTB = (PORTB & ~LEDS_MASK) | (rx[0] & LEDS_MASK);
				set_ready_light(rx[0] & READY_BIT);
				winner = (person)rx[1];
				set_light(winner);
			}
			//Reset reading index
			index = 0;
			break;

		// ====================================== //
		// SEND TO MASTER (ST, Slave Transmitter) //
		// ====================================== //

		// MASTER sent 0xA8: SLA+R Master went some bytes
		case TW_ST_SLA_ACK:
			//MASTER is alive
			slave_idle = 0;
			//Sending & resetting button flag
			TWDR = pending_press;
			pending_press = 0;
			break;
		
	


		// BUS ERROR
		case TW_BUS_ERROR:
			TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
			return ;
		// Otherwise, 0xC0, ...
		default:
			break;
	}

	//Trigger the actual response
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWEA) | (1 << TWIE);
}
