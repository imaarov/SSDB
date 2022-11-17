CC=gcc
CFLAGS=-Wall
ssdb: src/main.o
	$(CC) -o out/bin/ssdb src/main.o
