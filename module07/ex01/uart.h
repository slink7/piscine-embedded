#ifndef UART_H
#define UART_H

#include <avr/io.h>

#ifndef F_CPU
#define F_CPU 16000000UL
#endif

#ifndef UART_BAUDRATE
# define UART_BAUDRATE 115200UL
#endif

#define UBRR (F_CPU + UART_BAUDRATE * 8UL) / (16UL * UART_BAUDRATE) - 1

void uart_init(uint16_t ubrr);
void uart_tx(unsigned char data);
void uart_printstr(char *s);
uint8_t uart_rx(void);
void uart_flush(void);
void uart_print_byte(uint8_t byte);
void uart_print_ui32(uint32_t ui32);
void uart_print_pchar(char c);
void uart_read(uint8_t *buffer, uint8_t bsize);

#endif
