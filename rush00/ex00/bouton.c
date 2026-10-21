#include "rush.h"

static void	apply_press(person who)
{
	if (replay)	// On ne fait rien si la partie vient de se terminer (si on appuye pour le jeu on ne veut pas que ça compte comme "prêt")
		return;
	if (game_started && countdown_ended && winner == UNDEFINED)	// Si la partie est lancée, le bouton définit le gagnant
		set_winner(who);
	else if (game_started) // Si un clic est fait avant la fin du compteur, l'autre board gagne
	{
		if (who == SLAVE)
			set_winner(MASTER);
		else
			set_winner(SLAVE);
	}
	else if (!game_started) // On gère l'état "prêt"
	{
		if (who == SLAVE)
			is_slave_ready = 1;
		else if (who == MASTER)
			is_master_ready = 1;
		if (is_slave_ready && is_master_ready) // Si tout le monde est prêt on lance la partie
			start_game();
	}
}

// Appelee par le master a chaque tour avec les appuis de ce tour :
// appui local (SW1 du master) et appui du slave lu par master_poll.
// Si les deux arrivent dans le meme tour pendant la partie, aucun n'est
// passe en premier : c'est une egalite (sinon le master gagnerait toujours).
// L'interruption Timer1 modifie les memes variables (countdown,
// countdown_ended, replay...) : on coupe les interruptions le temps
// d'appliquer les appuis, puis on restaure SREG (bit I, datasheet : SREG).
void	buttons_called(uint8_t master_press, uint8_t slave_press)
{
	uint8_t	sreg = SREG;

	if (!master_press && !slave_press)
		return ;
	SREG &= ~(1 << SREG_I);
	if (master_press && slave_press && game_started)
		set_winner(TIE);
	else
	{
		if (master_press)
			apply_press(MASTER);
		if (slave_press)
			apply_press(SLAVE);
	}
	SREG = sreg;
}

void __vector_1(void)	// Interruption du bouton SW1, sans attente pour ne pas bloquer l'interruption TWI
{
	local_pressed = 1;
}

void init_button(void) // On set l'interruption du bouton SW1
{
	EICRA = (1 << ISC01);	// front descendant (appui, bouton actif a l'etat bas)
	EIFR = (1 << INTF0);
	EIMSK = (1 << INT0);
}

// Anti-rebond, appele a chaque tour de la boucle de main (toutes les 5 ms).
// Un appui n'est accepte que si :
//  - le bouton est bien enfonce au moment de la lecture (filtre les parasites)
//  - il etait relache depuis au moins DEBOUNCE_TICKS tours
// Apres un appui accepte, le compteur repart de 0 : les rebonds qui suivent
// ne peuvent pas compter comme un second appui.
uint8_t	button_pressed(void)
{
	static uint8_t	released = DEBOUNCE_TICKS;
	uint8_t			accepted = 0;
	uint8_t			is_up = PIND & (1 << PD2);	// 1 = relache (pull-up)

	if (local_pressed)
	{
		local_pressed = 0;
		if (!is_up && released >= DEBOUNCE_TICKS)
		{
			accepted = 1;
			released = 0;
		}
	}
	if (is_up)
	{
		if (released < DEBOUNCE_TICKS)
			released++;
	}
	else
		released = 0;
	return (accepted);
}
