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
void uart_tx(char data);
void uart_printstr(char *s);
char uart_rx(void);
void uart_flush(void);
void uart_print_byte(uint8_t byte);
void uart_print_u32(uint32_t ui32);
void uart_print_u16(uint16_t ui16);

void uart_print_uhex(uintmax_t umax, uint8_t len);
void uart_print_ihex(intmax_t imax, uint8_t len);
void uart_print_udec(intmax_t umax);
void uart_print_idec(intmax_t imax);

void uart_print_pchar(char c);
void uart_read(char *buffer, uint8_t bsize);

#endif
