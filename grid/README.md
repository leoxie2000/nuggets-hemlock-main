# Grid Module

## hemlock, May 2021 

### Grid

`grid` is a data structure needed for the nuggets game.

A `grid` stores character in a 2D array fashion.

Each character can be referenced using row and col values.

Supports functions to create a grid, load a map, set and get character values, and determine visible portion at a given point.

More detailed description provided in `grid.h`

### Grid Test

`gridtest.c` is a unit test for the grid.

The testing is done with `main.txt`

The test is limited to manually manipulating characters in the grid to test its functionality.

Visibility is manually tested with several points.

A more extensive testing will be possible once it is connected to `server` and `player`. 

### Makefile
 
 Target `all` creates `grid.a` library.
 
`Makefile` for the `grid` has two phony targets.

 Target `test` will compile `gridtest.c`.

 Target `clean` will clean the directory.
