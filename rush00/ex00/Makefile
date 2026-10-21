CC      = avr-gcc
OBJCOPY = avr-objcopy
AVRDUDE = avrdude

RM      = rm -f

# flags + id de la puce + frequence de la puce
CFLAGS  = -Wall -Wextra -Os -mmcu=$(MCU) -DF_CPU=$(F_CPU)

# Variables de la config materielle (infos de la puce)
MCU = atmega328p
F_CPU = 16000000UL
BAUD  = 115200

SRCS = main.c \
		bouton.c \
		game.c \
		init.c \
		leds.c \
		master.c \
		slave.c \


# Ports serie selon l'OS (macOS : /dev/cu.usbserial-* ou /dev/cu.wchusbserial*
# selon le driver CH340, Linux : /dev/ttyUSB*)
# PORT = une carte (flash), PORTS = toutes les cartes branchees (double)
UNAME := $(shell uname -s)
ifeq ($(UNAME), Darwin)
	PORTS ?= $(wildcard /dev/cu.usbserial-* /dev/cu.wchusbserial*)
	PORT ?= $(firstword $(PORTS))
else
	PORTS ?= $(wildcard /dev/ttyUSB*)
	PORT ?= /dev/ttyUSB0
endif

all: hex flash

hex: main.hex

main.bin: $(SRCS) rush.h
	$(CC) $(CFLAGS) -o main.bin $(SRCS)

# Génération du .hex à partir du .bin via avr-objcopy
main.hex: main.bin
	$(OBJCOPY) -O ihex main.bin main.hex

# Copie du .hex dans la mémoire flash des puces
flash: main.hex
ifeq ($(PORTS),)
	@echo "Aucune carte detectee : main.hex compile, flash ignore"
else
	for dev in $(PORTS); do\
		$(AVRDUDE) -c arduino -p $(MCU) -P $$dev -b $(BAUD) -U flash:w:main.hex:i || exit 1;\
	done
endif

clean:
	$(RM) main.bin main.hex

re: clean all

.PHONY: all hex flash double clean re
