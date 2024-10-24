# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	pio run

upload:
	pio run --target upload --target monitor

log:
	pio run --target monitor

clean:
	pio run --target clean

update:
	pio pkg update
