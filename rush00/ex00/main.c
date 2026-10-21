#include "rush.h"

// Global variables
volatile person player = UNDEFINED;
volatile person winner = UNDEFINED;
volatile uint8_t game_started = 0;
volatile uint8_t countdown_ended = 0;
volatile uint8_t is_slave_ready = 0;
volatile uint8_t is_master_ready = 0;
volatile uint8_t countdown = 0;
volatile uint8_t replay = 0;
volatile uint8_t pending_press = 0;
volatile uint8_t local_pressed = 0;
volatile uint8_t slave_idle = 0;

// Local variable
static uint8_t	missed_polls = 0;

// Master main logic
static void	master_loop(uint8_t local_press)
{
	uint8_t	slave_press = 0;

	if (master_poll(&slave_press))
		missed_polls = 0;
	else if (++missed_polls >= LINK_TIMEOUT_TICKS)
	{
		//Is the slave have not responded in a while, we reset
		reset_game();
		player = UNDEFINED;
		twi_init_slave();
		return ;
	}
	//Handle button presses for gameplay
	buttons_called(local_press, slave_press);
	//Toggle blue ready light
	set_ready_light(is_master_ready && !game_started);
}

// Slave main logic
static void	slave_loop(uint8_t local_press)
{
	if (player == SLAVE)
	{
		if (local_press)
			pending_press = 1;
		if (++slave_idle >= LINK_TIMEOUT_TICKS)
		{
			//If the master haven't ask for news in a while, we reset
			reset_game();
			pending_press = 0;
			player = UNDEFINED;
		}
	}
	else if (local_press)
	{
		// First to press goes from UNDEFINED to MASTER
		player = MASTER;
		missed_polls = 0;
		twi_init_master();
		buttons_called(1, 0);
	}
}

int main(void)
{
	uint8_t	pressed;

	init_led();
	init_button();
	// Both board are slaves until a master is self declared
	twi_init_slave();
	// Allows global interruption //sei()
	SREG |= (1 << SREG_I);

	while (1)
	{
		pressed = button_pressed();
		if (player == MASTER)
			master_loop(pressed);
		else
			slave_loop(pressed);
		_delay_ms(5);
	}
}
