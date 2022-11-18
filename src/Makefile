IDIR =../include
CC=gcc
CFLAGS=-I$(IDIR)
OUT=../out/bin/

ODIR=.
LDIR =../lib

LIBS=-lm

_DEPS = src/command.h src/header.h src/table_structure.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o 
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: %.c $(DEPS)
	$(CC) -c -o $(OUT)$@ $< $(CFLAGS)

ssdba11: $(OBJ)
	$(CC) -o $(OUT)$@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
	rm -f *.o 