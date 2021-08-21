/* grid.h - header file for the grid module
 *
 * grid is a data structure that holds character 2D array style 
 *
 * hemlock, May 2021
 */

#ifndef __GRID_H
#define __GRID_H

#include <stdio.h>
#include <stdbool.h>

/*********** global types *************/
typedef struct grid grid_t;

/*********** grid_new *************/
/* initializes a grid sized nrow*ncol.
 * returns a pointer to a grid.
 * returns NULL if negative nrow or ncol.
 * caller is responsible for freeing the grid using grid_delete().
 */
grid_t* grid_new(int nrow, int ncol);

/*********** grid_load *************/
/* loads a grid from a given filename.
 * returns pointer to a grid.
 * returns NULL if failed to open file or filename is invalid.
 * expects to recieve a valid map (textfile) as a parameter.
 */
grid_t* grid_load(char* mapFilename);

/*********** grid_nrow *************/
/* returns the number of row for the grid.
 * returns negative number if grid is NULL.
 */
int grid_nrow(grid_t* grid);

/*********** grid_ncol *************/
/* returns the number of col for the grid.
 * returns negative number if grid is NULL.
 */
int grid_ncol(grid_t* grid);

/*********** grid_getchar *************/
/* returns a character at the given row and col of the grid.
 * returns '^' if out of bounds or grid is NULL.
 */
char grid_getchar(grid_t* grid, int r, int c);

/*********** grid_update *************/
/* updates the character at the given row and col
 * Does nothing if grid is NULL, or if row and col is out of bounds.
 */
void grid_update(grid_t* grid, int r, int c, char ch);

/******************** grid_isEmptyRoomSpot *******************/
/* returns true if the character at given row and col is an empty room space.
 * returns false otherwise.
 * the caller should not pass a NULL grid
 */
bool grid_isEmptyRoomSpot(grid_t* grid, int r, int c);

/******************** grid_isPlayer *******************/
/* returns true if the character at given row and col is an empty room space with player.
 * returns false otherwise.
 * the caller should not pass a NULL grid
 */
bool grid_isPlayer(grid_t* grid, int r, int c);

/*********** grid_isGold *************/
/* returns true if the character at the given row and col is gold ('*').
 * returns false otherwise.
 * expects a non-NULL grid.
 */
bool grid_isGold(grid_t* grid, int r, int c);

/*********** grid_isBoundary *************/
/* returns true if the character at given row and col is boundary ('|', '-', '+').
 * returns false otherwise.
 * expects a non-NULL grid.
 */
bool grid_isBoundary(grid_t* grid, int r, int c);

/*********** grid_isRock *************/
/* returns true if the character at given row and col is ' '.
 * returns false if otherwise.
 * expects a non-NULL grid.
 */
bool grid_isRock(grid_t* grid, int r, int c);


/********************* grid_canMoveTo ***********************/
/* returns true if the character at given row and col is '.', 'A-Z', '*', '#' 
 * returns false if otherwise
 * the caller should not pass a NULL grid
 */
bool grid_canMoveTo(grid_t* grid, int r, int c);

/********************* grid_setVisibility ***********************/
/* updates the "known" grid based on the player's location (pr,pc)
 * Uses helper function grid_isVisible
 */
void
grid_setVisibility(grid_t* master, grid_t* raw, grid_t* known, int pr, int pc);

/*********** grid_toString *************/
/* Converts a grid to a string.
 * Returns a pointer to a string.
 * returns NULL if grid is NULL.
 * the caller is responsible for freeing the returned string.
 */
char* grid_toString(grid_t* grid);

/********************** grid_clean *************************/
/* cleans the "known" grid based on the raw grid 
 * Used at the bgeinning of grid_updateKnown
 */
void grid_clean(grid_t* raw, grid_t* known);

/*********** grid_delete *************/
/* Deletes a grid.
 */
void grid_delete(grid_t* grid);

#endif //__GRID_H
