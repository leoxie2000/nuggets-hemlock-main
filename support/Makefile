#
# Makefile for 'support.a'
# CS50 project Spring 2019
#
# David Kotz, May 2019
#

LIB = support.a
TESTS = messagetest

CFLAGS = -Wall -pedantic -std=c11 -ggdb
CC = gcc
MAKE = make

.PHONY: all clean

############# default rule ###########
all: $(LIB) $(TESTS) 

$(LIB): message.o log.o
	ar cr $(LIB) $^

messagetest: message.c message.h log.o
	$(CC) $(CFLAGS) -DUNIT_TEST message.c log.o -o messagetest

message.o: message.h
log.o: log.h

############# clean ###########
clean:
	rm -f core
	rm -rf *~ *.o *.gch *.dSYM
	rm -f *.log
	rm -f $(LIB)
	rm -f $(TESTS)
