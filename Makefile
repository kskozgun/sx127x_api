MCU_TYPE = atmega328p
CC = avr-gcc
SRC = src
OUTPUT = sensor_node


all: compile_src link hex

compile_src:
	$(CC) -g -Os -mmcu=$(MCU_TYPE) -c src/*.c

link:
	$(CC) -g -Os -mmcu=$(MCU_TYPE) -o $(OUTPUT).elf main.c *.o -I$(SRC)

hex:
	avr-objcopy -j .text -j .data -O ihex $(OUTPUT).elf $(OUTPUT).hex

flash:
	avrdude -p m328p -c usbtiny -U flash:w:$(OUTPUT).hex

size:
	avr-size -C --mcu=$(MCU_TYPE) $(OUTPUT).elf

clean:
	rm *.o
	rm *.elf
	rm *.hex