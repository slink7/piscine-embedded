#include "rush.h"

void	init_led(void)
{
	// Petites LEDs D1 a D4 (PB0, PB1, PB2, PB4) : compte a rebours
	DDRB |= (1 << DDB0);
	DDRB |= (1 << DDB1);
	DDRB |= (1 << DDB2);
	DDRB |= (1 << DDB4);

	DDRD |= (1 << DDD3);	// LED RGB bleue : joueur pret
	DDRD |= (1 << DDD5);	// LED RGB rouge : perdu
	DDRD |= (1 << DDD6);	// LED RGB verte : gagne
}

// Init du TWI en master a 100 kHz, aussi appelee apres un echange rate (master_poll)
void	twi_init_master(void)
{
	TWCR = 0;	// On coupe le TWI : sa machine d'etats repart a zero et le bus est relache
	TWBR = 72;	// Vitesse SCL = F_CPU / (16 + 2 * TWBR * prescaler) = 16 MHz / (16 + 2 * 72 * 1) = 100 kHz (datasheet : "Bit Rate Generator Unit")
	TWSR = 0x00;	// Bits TWPS a 0 -> prescaler 1
	TWCR = (1 << TWEN);	// On rallume le TWI
}

// Init du TWI en slave, les echanges sont geres par l'interruption TWI (ISR slave.c)
void	twi_init_slave(void)
{
	TWCR = 0;	// Reset du TWI (sortie propre du mode master)
	TWAR = SLAVE_ADDR << 1;	// Notre adresse, sur les bits 7..1 (datasheet : registre TWAR)
	TWCR = (1 << TWINT)	// On efface le flag d'interruption (en ecrivant 1)
		| (1 << TWEN)	// On active le TWI
		| (1 << TWEA)	// On repond ACK quand le master appelle notre adresse
		| (1 << TWIE);	// On active l'interruption TWI
}
