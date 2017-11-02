CC=gcc
CFLAGS = -c -g -Wall -Wextra
LFLAGS = -Wall -Wextra -pthread

.PHONY: all clean

all: multi-lookup

multi-lookup: multi-lookup.c multi-lookup.h
	$(CC) $(LFLAGS) $^ -o $@

clean:
	rm -f multi-lookup
	rm -f *.o
	rm -f *~
	rm -f result.txt serviced.txt 