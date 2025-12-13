CC = gcc
CFLAGS = -Wall -Werror -pedantic
SRC = src
BIN = bin

all: download

download: $(SRC)/download.c bin
	$(CC) $(CFLAGS) $(SRC)/download.c -o $(BIN)/download

bin:
	mkdir -p $(BIN)

clean:
	rm -rf $(BIN)/*

.PHONY: all clean