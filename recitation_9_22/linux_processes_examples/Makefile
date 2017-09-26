#Makefile
CC = gcc
INCLUDE = /usr/lib
LIBS = 
OBJS = 

all: master sqr cube fourth

master: master.c
	$(CC) -o master master.c $(CFLAGS) $(LIBS)
sqr: sqr.c
	$(CC) -o sqr sqr.c $(CFLAGS) $(LIBS)
cube: cube.c
	$(CC) -o cube cube.c $(CFLAGS) $(LIBS)
fourth: fourth.c
	$(CC) -o fourth fourth.c $(CFLAGS) $(LIBS)
clean:
	rm -f master sqr cube whole_num squares cubes fourth core
