OBJS = server.o
L1 = ../support
L2 = ../libcs50
L3 = ../grid
LLIBS1 = $(L1)/support.a
LLIBS2 = $(L2)/libcs50-given.a
LLIBS3 = $(L3)/grid.a
#TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$(L3) -I$(L2) -I$(L1)
CC = gcc
MAKE = make
all: server

server: $(OBJS) $(LLIBS3) $(LLIBS2) $(LLIBS1)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS3) $(LLIBS2) $(LLIBS1) -o server
	
server.o: $(L3)/grid.h $(L2)/mem.h $(L2)/file.h $(L1)/message.h $(L1)/log.h 
.PHONY: clean

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f server
	rm -f core
	rm -f vgcore.*
