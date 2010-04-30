# Makefile for the Spreadsheet

BINARIES  = spreadsheet 
CC        =  gcc -Wall -g -std=c99
LIBRARIES = `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
RM        = rm -rf

all: $(BINARIES)

%: %.c
	$(CC) -o $@ $< $(LIBRARIES)

.PHONY: clean
clean: 
	$(RM) $(BINARIES)


