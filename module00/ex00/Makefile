
BOARD=atmega328p

all: hex flash

hex:
	avr-gcc -mmcu=$(BOARD) -DF_CPU=16000000UL -o main.bin main.c
	avr-objcopy -O ihex -R .eeprom main.bin main.hex

flash:
	avrdude -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex -p m328p

clean:
	rm main.bin main.hex

.PHONY: hex flash all
