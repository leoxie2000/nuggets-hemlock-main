# Makefile for 'querier' 
#
# Yi Xie, April, 2021
#

OBJS = player.o
LIBS = -lncurses
L = ../support
LLIBS = $L/support.a

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L 
CC = gcc
MAKE = make
all: player

player: $(OBJS) $(LLIBS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) $(LIBS) -o player


player.o: $L/message.h $L/log.h
.PHONY: test valgrind clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f player
	rm -f core
	rm -f vgcore*
	rm -f playerlog

