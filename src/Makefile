CC = gcc
CFLAGS = -Wall -Werror -pedantic
ROOT = ..
BIN = $(ROOT)/bin

all: download

download: download.c bin
	$(CC) $(CFLAGS) download.c -o $(BIN)/download

bin:
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)/*

.PHONY: all clean