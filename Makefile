CC=gcc
CFLAGS=-Wall
DEPS = src/command.h src/header.h src/table_structure.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

ssdb: src/main.o 
	$(CC) -o out/bin/Ssdb src/main.o 