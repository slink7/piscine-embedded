#include <avr/io.h>
#include <stddef.h>
#include <stdint.h>
#include <util/crc16.h>

#include "uart.h"
#include "eeprom.h"
#include "utils.h"

#define INPUT_SIZE 42
#define MAGIC 0x7F
#define NOT_SET 0xFFFF

#define OFFSET_ID		0x1
#define OFFSET_PRIORITY	0x5
#define OFFSET_TAG		0x7
#define OFFSET_CRC		0x40

const uint16_t node_slots[] = {
	0x064, 0x111, 0x282, 0x300
};
const uint8_t slot_count = sizeof(node_slots) / sizeof(uint16_t);

uint16_t current_slot = NOT_SET;

typedef struct node {
	uint8_t		magic;
	uint32_t	id;
	int16_t		priority;
	char		tag[33];
	uint16_t	crc;
} node; //size 42

node g_n = {};

void uart_mem(void *ptr, uint8_t len) {
	for (uint8_t k = 0; k < len; ++k) {
		uart_print_byte(((uint8_t*)ptr)[k]);
		uart_printstr(" ");
	}
}

uint8_t eeprom_write_node(uint16_t addr, node *n) {
	if (!n)
		return (0);
	uart_printstr("Writing to eeprom\r\n");
	return (eeprom_memcpy_w(addr, n, sizeof(node)));
}

uint8_t eeprom_read_node(uint16_t addr, node *n) {
	if (!n)
		return (0);
	eeprom_memcpy_r(addr, n, sizeof(node));
	return (1);
}

uint16_t crc16(const uint8_t *data, uint8_t len) {
	uint16_t crc = 0xFFFF;

	for (uint8_t k = 0; k < len; ++k) {
		crc ^= data[k];
		for (uint8_t l = 0; l < 8; ++l) {
			if (crc & 0x8000) {
				crc = ((crc << 1) ^ 0x1DCF);
			} else {
				crc = (crc << 1);
			}
		}
	}
	return (crc);
}

// uint16_t crc16(uint8_t *data, uint8_t len) {
// 	uint16_t crc = 0xFFFF;
//
// 	for (uint8_t k = 0; k < len; ++k) {
// 		crc = _crc16_update(crc, data[k]);
// 	}
// 	return (crc);
// }

uint16_t node_crc16(node *n) {
	return (crc16(((uint8_t *)n) + sizeof(uint8_t), sizeof(node) - sizeof(uint8_t) - sizeof(uint16_t)));
}

void locate_data() {
	current_slot = NOT_SET;
	for (uint8_t k = 0; k < slot_count; ++k) {
		if (eeprom_read(node_slots[k]) != MAGIC)
			continue ;
		eeprom_read_node(node_slots[k], &g_n);
		if (node_crc16(&g_n) == g_n.crc && g_n.magic == MAGIC) {
			current_slot = node_slots[k];
			return ;
		}
	}
}

void save_gnode() {
	if (current_slot == NOT_SET) {
		current_slot = node_slots[0];
		g_n.magic = MAGIC;
	}

	g_n.crc = node_crc16(&g_n);

	uint8_t success = eeprom_write_node(current_slot, &g_n);

	if (success)
		return ;
	uart_printstr("Corruption detected.\r\n");
	for (uint8_t k = 0; k < slot_count; ++k) {
		uart_printstr("Relocating config to slot ");
		uart_print_byte(k);
		uart_printstr("... ");

		success = eeprom_write_node(node_slots[k], &g_n);
		if (success) {
			uart_printstr("Success\r\n");
			current_slot = node_slots[k];
			break ;
		}

		uart_printstr("Fail\r\n");
		if (k == (slot_count - 1)) {
			uart_printstr("CRITICAL EEPROM FAILURE.\r\n");
			current_slot = NOT_SET;
			return ;
		}
	}
	uart_printstr("Done.");
}

uintmax_t parse_udec(char *str) {
	uintmax_t out = 0;

	for (uint8_t k = 0; (str[k] >= '0') && (str[k] <= '9'); ++k)
		out = out * 10 + str[k] - '0';
	return (out);
}

uintmax_t parse_idec(char *str) {
	uintmax_t out = 0;
	uint8_t	sign = 0;

	if (str[0] == '-')
		sign = 1;
	out = parse_udec(str + sign);
	return (sign ? -out : out);
}

int8_t parse_i16_dec(int16_t *out, char *str) {
	intmax_t tmp = parse_idec(str);

	if (tmp > INT16_MAX || tmp < INT16_MIN)
		return (0);

	*out = (int16_t)tmp;
	return (1);
}

int8_t parse_u32_dec(uint32_t *out, char *str) {
	intmax_t tmp = parse_udec(str);
	
	if (tmp > UINT32_MAX)
		return (0);

	*out = (int32_t)tmp;
	return (1);
}

uint8_t is_valid(char *str, uint8_t len) {
	for (uint8_t k = 0; str[k] && k < len; ++k) {
		if (
			!((str[k] >= 'A' && str[k] <= 'Z')
			|| (str[k] >= 'a' && str[k] <= 'z')
			|| (str[k] >= '0' && str[k] <= '9')
			|| str[k] == '-' || str[k] == '_')
		)
			return (0);
	}
	return (1);
}

uint8_t handle_command(char *cmd) {
	if (!ft_strncmp(cmd, "STATUS", 7)) {
		// ===========================================
		//		STATUS
		// ===========================================

		if (current_slot == NOT_SET) {
			uart_printstr("Node unconfigured\r\n");
			return (0);
		}

		eeprom_read_node(current_slot, &g_n);
		if (node_crc16(&g_n) != g_n.crc) {
			uart_printstr("CRITICAL: Data corruption detected!\r\n");
			uart_print_uhex(node_crc16(&g_n), 4);
			uart_printstr(" != ");
			uart_print_uhex(g_n.crc, 4);
			uart_printstr("\r\n");
			return (0);
		}

		uart_printstr("Status: OK\r\n");
		uart_printstr("Current Slot: 0x");
		uart_print_u16(current_slot);
		uart_printstr("\r\n");

		uart_printstr("ID: ");
		uart_print_udec(g_n.id);
		uart_printstr("\r\n");

		uart_printstr("priority: ");
		uart_print_idec(g_n.priority);
		uart_printstr("\r\n");

		uart_printstr("Tag: ");
		uart_printstr(g_n.tag);
		uart_printstr("\r\n");

	} else if (!ft_strncmp(cmd, "SET_ID ", 7)) {
		// ===========================================
		//		SET_ID
		// ===========================================
		
		if (parse_u32_dec(&g_n.id, cmd + 7)) {
			uart_printstr("Set ID to ");
			uart_print_udec(g_n.id);
			uart_printstr("\r\n");
			return (1);
		}

		uart_printstr("Syntax Error: u32 out of bounds\r\n");

	} else if (!ft_strncmp(cmd, "SET_PRIO ", 9)) {
		// ===========================================
		//		SET_PRIO
		// ===========================================

		if (parse_i16_dec(&g_n.priority, cmd + 9)) {
			uart_printstr("Set priority to ");
			uart_print_idec(g_n.priority);
			uart_printstr("\r\n");
			return (1);
		}

		uart_printstr("Error: i16 out of bounds\r\n");

	} else if (!ft_strncmp(cmd, "SET_TAG ", 8)) {
		// ===========================================
		//		SET_TAG
		// ===========================================
		if (cmd[8] != '"') {
			uart_printstr("Syntax Error: missing first '\"'\r\n");
			return (0);
		}
		uint8_t len = ft_strlen(cmd + 9);
		if (cmd[9 + len - 1] != '"') {
			uart_printstr("Syntax Error: error with 2nd '\"'\r\n");
			return (0);
		}
		if (!is_valid(cmd + 9, len - 1)) {
			uart_printstr("Syntax Error: use of invalid chars\r\n");
			return (0);
		}
		ft_memcpy(&g_n.tag, cmd + 9, len - 1);
		g_n.tag[len - 1] = '\0';
		return (1);

	} else if (!ft_strncmp(cmd, "FACTORY_RESET", 15)) {
		// ===========================================
		//		FACTORY_RESET
		// ===========================================

		if (current_slot != NOT_SET)
			eeprom_write(current_slot, MAGIC + 1);
		
		current_slot = NOT_SET;
		ft_memset(&g_n, 0, sizeof(node));
		return (0);
		
	} else if (!ft_strncmp(cmd, "PRINT", 6)) {
		// ===========================================
		//		PRINT
		// ===========================================
		eeprom_hexdump_C();
	} else if (!ft_strncmp(cmd, "DEBUG ", 6)) {
		// ===========================================
		//		DEBUG
		// ===========================================
		
		uint8_t arg = (cmd[6] >= '0' && cmd[6] <= '9')
			? cmd[6] - '0'
			: (cmd[6] >= 'A' && cmd[6] <= 'F')
			? cmd[6] - 'A' + 10
			: (cmd[6] >= 'a' && cmd[6] <= 'f')
			? cmd[6] - 'a' + 10
			: 0xFF
		;

		if (arg == 0xFF || cmd[7] != '\0') {
			uart_printstr("Syntax Error\r\n");
			return (0);
		}

		debug_corrution = arg;
			
	} else {
		uart_printstr("\t\tINVALID COMMAND\r\n");
		uart_printstr(cmd);
		uart_printstr("\r\n");
	}
	return (0);
}

int main(void) {
	uart_init(UBRR);

	ft_memset(&g_n, 0, sizeof(node));
	locate_data();

	char input[INPUT_SIZE + 1] = {};
	while (1) {
		uart_printstr("=> ");
		uart_read(input, INPUT_SIZE);
		uart_printstr("\n\r");
		if (handle_command(input))
			save_gnode();
		// eeprom_hexdump_C();
	}
	return (0);
}
