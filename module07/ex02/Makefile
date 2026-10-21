
F_CPU=16000000UL
UART_BAUDRATE=115200
DEVICE=/dev/ttyUSB0
MCU=atmega328p

NAME=main

CFLAGS=\
	-mmcu=atmega328p\
	-DF_CPU=$(F_CPU)\
	-DUART_BAUDRATE=$(UART_BAUDRATE)\
	-I/usr/avr/include\
	-Os\
	-Wall -Wextra\
	-MMD -MP

SRCS=\
	main.c\
	uart.c\
	eeprom.c\
	utils.c

DEPS=$(SRCS:.c=.d)

OBJS=$(SRCS:.c=.o)

all: flash clean

%.hex: %.bin
	avr-objcopy -O ihex $< $@

$(NAME).bin: $(OBJS)
	avr-gcc\
		-mmcu=$(MCU)\
		$(OBJS)\
		-o $@

%.o: %.c
	avr-gcc $(CFLAGS) -c $< -o $@

flash: $(NAME).hex
	for dev in $(shell ls /dev/ttyUSB*); do\
		avrdude\
			-c arduino\
			-P $$dev\
			-b $(UART_BAUDRATE)\
			-U flash:w:$(NAME).hex\
			-p $(MCU);\
	done

clean:
	rm -f $(OBJS) $(DEPS) $(NAME).bin $(NAME).hex

-include $(DEPS)

upload: all clean

hd: upload
	stty -F $(DEVICE) $(UART_BAUDRATE) raw cs8 -cstopb -parenb
	hexdump -Cv /dev/ttyUSB0

cat: upload
	stty -F $(DEVICE) $(UART_BAUDRATE) raw cs8 -cstopb -parenb
	cat /dev/ttyUSB0

screen: upload
	screen $(DEVICE) $(UART_BAUDRATE)


.PHONY: clean upload hex flash all
