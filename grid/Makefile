# Makefile for the grid module
# hemlock, May, 2021

# define directories
L = ../libcs50
#C = /common

# object files and libraries
OBJS = grid.o
LIB = grid.a
LLIBS = $L/libcs50-given.a   

# uncomment for memtest or debugging
#TESTING=-DMEMTEST
#TESTING=-DDEBUG


CFLAGS = -Wall -pedantic -std=c11 -ggdb -I$L
CC = gcc
MAKE = make

$(LIB): $(OBJS) $(LLIBS)
	 ar cr $(LIB) $(OBJS)

gridtest: gridtest.o
	$(CC) $(CFLAGS) $^ $(LIB) $(LLIBS) -o $@

# dependencies: object files depend on header files
grid.o: grid.h $L/mem.h $L/file.h

gridtest.o: grid.h $L/mem.h $L/file.h

.PHONY: clean test

test: gridtest

# clean up after compilation
clean:
	 rm -f core 
	 rm -f grid
	 rm -f gridtest
	 rm -f grid.a
	 rm -f *~ *.o
	 rm -rf *.dSYM
