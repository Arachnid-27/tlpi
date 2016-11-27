include ../Makefile.inc

SRC=$(wildcard *.c)
EXE=$(patsubst %.c,%.out,$(SRC))

all: $(EXE)

%.out: %.c 
	$(CC) $< $(CFLAGS) -o $@

clean:
	rm -f $(EXE)
