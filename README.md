# CS50 Nuggets-Hemlock

### Group Members
- Leo(Yi) Xie
- Jeff Yang
- Tim Kim
- Tay Kim

### Summary of Each Directory


#### grid

`grid` is a data structure needed for the nuggets game that stores character in a 2D array fashion.

`make` creates `grid.a`, `make test` runs the unit test file `gridtest.c` and `make clean` removes any output files that have been created Please refer to `grid.h` for further implementation details. 

#### server

Contains `server.c` and `Makefile`. `server.c` is the server side of the nuggets game, being in charge of message sending, receiving, and holding game status. `Makefile` supports compiling.



#### player

Contains `Makefile` and `player.c`. `player.c` has the client side of the game, and `Makefile` supports compiling.


#### support

The support library contains the message module `message.c` and log module `log.c`. For details, look into that directory's `.h` files.

#### libcs50

Another supporting library provided.
