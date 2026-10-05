
F_CPU=16000000UL

all: hex flash

hex:
	avr-gcc -mmcu=atmega328p -Os -DF_CPU=$(F_CPU) -I/usr/avr/include -o main.bin main.c
	avr-objcopy -O ihex main.bin main.hex

flash:
	avrdude -c arduino -P /dev/ttyUSB0 -b 115200 -U flash:w:main.hex -p m328p

clean:
	rm main.bin main.hex

upload: all clean

.PHONY: clean upload hex flash all
