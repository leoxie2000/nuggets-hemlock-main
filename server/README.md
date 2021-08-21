# Server

## hemlock May, 2021

### Server
* `server.c` the server program

Implement all the game logic.

For more information, please refer to "Design Specification" and "implementation spec".

All the specs are met, and all the functions works well.



### Usage
```
./server map [seed]
```
* `map` is the path for a valid map, where it has to be valid, see Spec for more information
* `[seed]` optional seed for the random behavior


### Makefile
* `Makefile` makefile for the server

To compile the program
```bash
make
```

To clean
```bash
make clean
```