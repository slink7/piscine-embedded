#include <avr/io.h>
#include <stdint.h>
#include <util/crc16.h>

#include "uart.h"
#include "eeprom.h"

#define INPUT_SIZE 42
#define MAGIC 0x7F
#define NOT_SET 0xFFFF

const uint16_t node_slots[] = {
	0x042, 0x111, 0x2B2, 0x300
};

uint16_t current_slot = NOT_SET;

typedef struct node {
	uint8_t		magic;
	uint32_t	id;
	int16_t		priority;
	char		tag[33];
	uint16_t	crc;
} node; //size 42


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

void locate_data(node *n) {
	current_slot = NOT_SET;
	for (uint8_t k = 0; k < (sizeof(node_slots) / sizeof(uint16_t)); ++k) {
		if (eeprom_read(node_slots[k]) != MAGIC)
			continue ;
		eeprom_read_node(node_slots[k], n);
		if (node_crc16(n) == n->crc && n->magic == MAGIC) {
			current_slot = node_slots[k];
		}
	}
}

int main(void) {
	
	node n = {
		0x7F,
		0x400,
		-142,
		"scambier was here\0yolo",
		0
	};

	n.crc = node_crc16(&n);

	locate_data(&n);

	uart_init(UBRR);

	uint8_t input[INPUT_SIZE + 1] = {};
	while (1) {
		uart_printstr("=> ");
		uart_read(input, INPUT_SIZE);
		uart_printstr("\n\r");
		// eeprom_write_node(0x2B2, &n);
		eeprom_hexdump_C();
	}
	return (0);
}
