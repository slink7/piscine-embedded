#ifndef RUSH_H
#define RUSH_H

#include <avr/io.h>
#include <util/delay.h>
#include <util/twi.h>

// Duree de la phase resultat, en ticks de 750 ms (4 = 3 s)
# define RESULT_TICKS 4

// Anti-rebond : bouton relache pendant au moins 4 tours de 5 ms (20 ms)
# define DEBOUNCE_TICKS 4

// Perte de liaison : 40 tours de 5 ms (~200 ms) sans echange reussi
# define LINK_TIMEOUT_TICKS 40

// Adresse I2C (7 bits) d'une carte en mode slave.
// Le bus contient deja d'autres composants (schema du devkit) :
// PCA9555 en 0x20-0x27, AHT20 en 0x38, RTC PCF8563 en 0x51.
// 0x00-0x07 et 0x78-0x7F sont reserves par la norme I2C.
# define SLAVE_ADDR 0x10

// LEDs du compte a rebours (D1 a D4)
# define LEDS_MASK ((1 << PB0) | (1 << PB1) | (1 << PB2) | (1 << PB4))

// Bit libre (bit 7) de l'octet LEDs envoye au slave : 1 = joueur slave pret
# define READY_BIT (1 << 7)

void __vector_1(void) __attribute__((signal, used, externally_visible));
void __vector_11(void) __attribute__((signal, used, externally_visible));
void __vector_24(void) __attribute__((signal, used, externally_visible));

typedef enum {
	SLAVE,
	MASTER,
	UNDEFINED,
	TIE		// egalite : les deux joueurs ont appuye dans le meme tour
} person;

extern volatile person player;
extern volatile person winner;
extern volatile uint8_t game_started;
extern volatile uint8_t countdown_ended;
extern volatile uint8_t is_slave_ready;
extern volatile uint8_t is_master_ready;
extern volatile uint8_t countdown;
extern volatile uint8_t replay;
extern volatile uint8_t pending_press;
extern volatile uint8_t local_pressed;
extern volatile uint8_t slave_idle;

// Derniere etape et dernier code de statut des echanges du master (master.c)
extern uint8_t twi_step;
extern uint8_t twi_last_status;

void	compte_a_rebours(void);
void	buttons_called(uint8_t master_press, uint8_t slave_press);
void	set_winner(person player);
void	reset_game(void);
void	set_light(person winner);
void	set_ready_light(uint8_t on);
void	start_game(void);
void	init_led(void);
void	init_button(void);
uint8_t	button_pressed(void);
uint8_t	twi_master_start(void);
uint8_t	twi_master_write(uint8_t data, uint8_t expected);
void	twi_master_stop(void);
uint8_t	twi_read_nack(uint8_t *data);
uint8_t	twi_send_state(uint8_t leds, uint8_t w);
uint8_t	twi_read_press(uint8_t *pressed);
void	twi_init_master(void);
void	twi_init_slave(void);
uint8_t	master_poll(uint8_t *slave_pressed);

#endif
