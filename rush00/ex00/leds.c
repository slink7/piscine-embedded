#include "rush.h"

// LED RGB (schema : rouge PD5, vert PD6, bleu PD3) : vert = gagne,
// rouge = perdu, jaune = egalite, eteinte = pas de resultat
void	set_light(person winner)
{
	if (winner == UNDEFINED) // On coupe les leds
	{
		PORTD &= ~(1 << PD6);
		PORTD &= ~(1 << PD5);
	}
	else if (winner == TIE)	// egalite : jaune (rouge + vert)
	{
		PORTD |= (1 << PD5);
		PORTD |= (1 << PD6);
	}
	else if (winner == player) // board gagnante -> led verte
	{
		PORTD &= ~(1 << PD5);
		PORTD |= (1 << PD6);
	}
	else	// led rouge
	{
		PORTD &= ~(1 << PD6);
		PORTD |= (1 << PD5);
	}
}

// LED RGB bleue (PD3) : allumee quand le joueur de cette carte est pret
// et attend l'autre joueur
void	set_ready_light(uint8_t on)
{
	if (on)
		PORTD |= (1 << PD3);
	else
		PORTD &= ~(1 << PD3);
}
