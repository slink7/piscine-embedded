#include <avr/io.h>
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
	0x042, 0x111, 0x2B2, 0x300
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

uint8_t eeprom_write_node(uint16_t addr, node *n) {
	if (!n)
		return (0);
	return (eeprom_memcpy_w(addr, n, sizeof(node)));
}

uint8_t eeprom_read_node(uint16_t addr, node *n) {
	if (!n)
		return (0);
	eeprom_memcpy_r(addr, n, sizeof(node));
	return (1);
}

// uint16_t crc16(const uint8_t *data, uint8_t len) {
// 	uint16_t crc = 0xFFFF;
//
// 	for (uint8_t k = 0; k < len; ++k) {
// 		crc ^= data[k];
// 		for (uint8_t l = 0; l < 8; ++l) {
// 			if (crc & 0x8000) {
// 				crc = ((crc << 1) ^ 0x1DCF);
// 			} else {
// 				crc = (crc << 1);
// 			}
// 		}
// 	}
// 	return (crc);
// }

uint16_t crc16(uint8_t *data, uint8_t len) {
	uint16_t crc = 0xFFFF;

	for (uint8_t k = 0; k < len; ++k) {
		crc = _crc16_update(crc, data[k]);
	}
	return (crc);
}

uint16_t node_crc16(node *n) {
	return (crc16(((uint8_t *)&n) + sizeof(uint8_t), sizeof(node) - sizeof(uint8_t) - sizeof(uint16_t)));
}

void locate_data() {
	node n;

	current_slot = NOT_SET;
	for (uint8_t k = 0; k < slot_count; ++k) {
		uart_printstr("mgk: ");
		uart_print_byte(eeprom_read(node_slots[k]));
		uart_printstr("\n\r");
		if (eeprom_read(node_slots[k]) != MAGIC)
			continue ;
		uart_printstr("Found Magic\r\n");
		eeprom_read_node(node_slots[k], &n);
		if (node_crc16(&n) == n.crc && n.magic == MAGIC) {
			current_slot = node_slots[k];
			uart_printstr("Found slot\r\n");
			return ;
		}
	}
}

void save_gnode() {
	if (current_slot == NOT_SET)
		current_slot = node_slots[0];

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
			break ;
		}

		uart_printstr("Fail\r\n");
		if (k == (slot_count - 1)) {
			uart_printstr("CRITICAL EEPROM FAILURE.\r\n");
			return ;
		}
	}
	uart_printstr("Done.");
}

uint8_t parse_udec(uintmax_t *out, char *str, uintmax_t max) {
	*out = 0;
	for (uint8_t k = 0; str[k] >= '0' && str[k] <= '9'; ++k) {
		if (*out > (max + '0' - str[k]) / 10)
			return (0);
		*out = *out * 10 + str[k] - '0';
	}
	return (1);
}

intmax_t parse_idec(intmax_t *out, char *str) {
	uintmax_t tmp = 0;

	if (!parse_udec(&tmp, str + (str[0] == '-')))
		return (0);
	*out = (str[0] == '-') ? -tmp : tmp;
	return (1);
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

	if (tmp > INT32_MAX)
		return (0);
	*out = (uint32_t)tmp;
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
			return (0);
		}

		uart_printstr("Status: OK\r\n");
		uart_printstr("Current Slot: 0x");
		uart_print_u16(current_slot);
		uart_printstr("\r\n");

		uart_printstr("ID: ");
		uart_print_uhex(g_n.id, 4);
		uart_printstr("\r\n");

		uart_printstr("priority: ");
		uart_print_ihex(g_n.priority, 4);
		uart_printstr("\r\n");

		uart_printstr("Tag: ");
		uart_printstr(g_n.tag);
		uart_printstr("\r\n");

	} else if (!ft_strncmp(cmd, "SET_ID ", 7)) {
		// ===========================================
		//		SET_ID
		// ===========================================
			

	} else if (!ft_strncmp(cmd, "SET_PRIO ", 9)) {
		// ===========================================
		//		SET_PRIO
		// ===========================================
		

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
		ft_memcpy(&g_n.tag, cmd + 9, len - 1);
		return (1);

	} else if (!ft_strncmp(cmd, "FACTORY_RESET", 15)) {
		// ===========================================
		//		FACTORY_RESET
		// ===========================================

	} else if (!ft_strncmp(cmd, "PRINT", 6)) {
		// ===========================================
		//		PRINT
		// ===========================================
		eeprom_hexdump_C();
	} else {
		uart_printstr("\t\tINVALID COMMAND\r\n");	
	}
	return (0);
}

int main(void) {
	uart_init(UBRR);

	locate_data();

	char input[INPUT_SIZE + 1] = {};
	while (1) {
		uart_printstr("=> ");
		uart_read(input, INPUT_SIZE);
		uart_printstr("\n\r");
		handle_command(input);
		
		// eeprom_hexdump_C();
	}
	return (0);
}
