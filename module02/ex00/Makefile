
F_CPU=16000000UL
UART_BAUDRATE=115200
DEVICE=/dev/ttyUSB0

all: hex flash

hex:
	avr-gcc -mmcu=atmega328p -Os -DF_CPU=$(F_CPU) -DUART_BAUDRATE=$(UART_BAUDRATE) -I/usr/avr/include -o main.bin main.c
	avr-objcopy -O ihex main.bin main.hex

flash:
	avrdude -c arduino -P $(DEVICE) -b $(UART_BAUDRATE) -U flash:w:main.hex -p m328p

clean:
	rm main.bin main.hex

upload: all clean

uart: upload
	# screen $(DEVICE) $(UART_BAUDRATE)
	stty -F $(DEVICE) $(UART_BAUDRATE) raw cs8 -cstopb -parenb
	hexdump -Cv /dev/ttyUSB0

.PHONY: clean upload hex flash all
