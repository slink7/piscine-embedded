#ifndef SPI_H
#include <avr/io.h>

void spi_master_init();
void spi_master_transmit(uint8_t data);
void spi_master_transmit_u32(uint32_t data);

void sk9822_start();
void sk9822_stop();
void sk9822_set_led(uint32_t led);
void sk9822_set_leds(uint32_t *leds, uint8_t len);
void sk9822_set_rgb(uint8_t brightness, uint8_t r, uint8_t g, uint8_t b);void spi_master_init();

#endif
