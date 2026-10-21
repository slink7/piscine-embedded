#include "rush.h"

// Timer1 en mode CTC, prescaler 1024 : 16 MHz / 1024 * 0.75 s - 1 = 11717
// -> une interruption toutes les 750 ms$
static void	start_timer(void)
{
	TCCR1A = 0;
	TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);
	TCNT1 = 0;
	OCR1A = 11717;

	// Efface une comparaison restee en attente de l'ancien comptage
	// (sinon l'interruption tomberait tout de suite) : on ecrit 1 sur OCF1A
	TIFR1 = (1 << OCF1A);
	TIMSK1 = (1 << OCIE1A);
}

static void	stop_timer(void) // On coupe les interruptions
{
	TIMSK1 = 0;
	TCCR1B = 0;
}

// Fin de la phase resultat : on eteint tout et on peut rejouer
static void	end_result(void)
{
	stop_timer(); // On arrête le timer
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // On coupe les leds
	set_light(UNDEFINED);	// On coupe les leds "gagnant" / "perdant"
	winner = UNDEFINED;	// On reset les variables de jeu
	countdown = 0;
	replay = 0;
}

// Annule la partie en cours (perte de liaison) : tout revient a l'etat du demarrage
void	reset_game(void)
{
	uint8_t	sreg = SREG;

	SREG &= ~(1 << SREG_I);
	stop_timer();
	PORTB &= ~LEDS_MASK;
	set_light(UNDEFINED);
	set_ready_light(0);
	winner = UNDEFINED;
	game_started = 0;
	countdown_ended = 0;
	is_master_ready = 0;
	is_slave_ready = 0;
	countdown = 0;
	replay = 0;
	SREG = sreg;
}

// ISR TIMER1_COMPA (vecteur 11)
void __vector_11(void)
{
	// Phase resultat : RESULT_TICKS x 750 ms puis retour a l'attente
	if (replay)
	{
		countdown++;
		if (countdown >= RESULT_TICKS)
			end_result();
		return;
	}

	switch (countdown) // On éteint chaque led puis on les rallume toutes, ensuite le jeu commence
	{
	case 0:
		PORTB &= ~(1 << PB0);
		break;
	case 1:
		PORTB &= ~(1 << PB1);
		break;
	case 2:
		PORTB &= ~(1 << PB2);
		break;
	case 3:
		PORTB &= ~(1 << PB4);
		break;
	case 4:
		PORTB |= (1 << PB0);
		PORTB |= (1 << PB1);
		PORTB |= (1 << PB2);
		PORTB |= (1 << PB4);
		stop_timer();
		countdown_ended = 1; // On signale le début du jeu
		break;
	default:
		break;
	}
	countdown++;
}

void	compte_a_rebours(void) // On relance le compte a rebours -> on rallume les leds, remet le compteur à zero et lance le timer
{
	PORTB |= (1 << PB0);
	PORTB |= (1 << PB1);
	PORTB |= (1 << PB2);
	PORTB |= (1 << PB4);

	countdown = 0;
	start_timer();
}

void	start_game(void) // On remet toutes les variables à 0 et on lance le compte à rebours
{
	if (is_master_ready && is_slave_ready)
	{
		game_started = 1;
		countdown_ended = 0;
		winner = UNDEFINED;
		compte_a_rebours();
	}
}

void	set_winner(person player)
{
	if (winner == UNDEFINED)
		winner = player;

	// On n'affiche que le resultat sur la LED RGB
	PORTB &= ~((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4)); // On éteint les petites leds
	set_light(winner); // On set la led rgb en fonction du gagnant

	// On remet les variables à 0
	game_started = 0;
	is_master_ready = 0;
	is_slave_ready = 0;
	countdown_ended = 0;

	// Phase resultat : on relance Timer1, les appuis sont ignores
	// jusqu'a end_result()
	countdown = 0;
	replay = 1;
	start_timer();
}
