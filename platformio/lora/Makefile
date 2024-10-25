# Uncomment lines below if you have problems with $PATH
#SHELL := /bin/bash
#PATH := /usr/local/bin:$(PATH)

all:
	pio run

compile_commands.json:
	./create-conf.sh
	make db

flash: upload

upload:
	pio run --target upload --target monitor

log:
	pio run --target monitor

clean:
	pio run --target clean

update:
	pio pkg update

db: 
	pio run -t compiledb
