# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all: ccloader
	pio run

flash-esp:
	pio run --target upload

flash-cc2531: ccloader
	./ccloader /dev/ttyUSB0 CC2531ZNP-Prod.bin 1

clean:
	pio run --target clean

update:
	pio pkg update

ccloader: ccloader.c
	$(CC) -o ccloader ccloader.c
