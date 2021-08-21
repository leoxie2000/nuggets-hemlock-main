/*
 * grid.c - grid module
 *
 * hemlock, May 2021
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include "mem.h"
#include "file.h"
#include "grid.h"

/**************** global type ********************/
typedef struct grid {
 char* map; //pointer to the character at (0,0)
 int nrow; 
 int ncol;
} grid_t;

/************* Local Function Prototypes ******************/
static bool grid_isVisible(grid_t* master, int pr, int pc, int r, int c);
static bool grid_isBlockable(grid_t* grid, int r, int c);

/********************** grid_new **************************/
/* initializes a grid, caller responsible for freeing the grid by grid_delete()
 * returns a pointer to a grid.
 * returns NULL if negative row or column is provided as paramater.
 */
grid_t* 
grid_new (int nrow, int ncol)
{ 
  if(nrow < 0 || ncol < 0){ //negative row or col
    return NULL;
  }

  grid_t* grid = mem_malloc_assert(sizeof(grid_t), "out of memory");

  grid->nrow = nrow;
  grid->ncol = ncol;

  grid->map = mem_calloc_assert(nrow*ncol, sizeof(char), "out of memory"); //allocate size for nrow*ncol characters

  //set every character as ' '
  for(int r = 0; r < nrow; r++){
    for(int c = 0; c < ncol; c++){
      *(grid->map + r*ncol + c) = ' ';
    }
  }
  return grid;
}

/*********************** grid_load ************************/
/* loads a grid from a given filename.
 * returns a pointer to a grid.
 * returns NULL if failed to open file or filename is invalid.
 * caller is responsible for later freeing the grid using grid_delete()
 */
grid_t*
grid_load (char* mapFilename)
{
  FILE* fp;
  char* pathname = NULL;
  pathname = mem_malloc_assert(strlen(mapFilename)+1, "out of memory\n");
  
  //read file name and consturct directory path
  if(sprintf(pathname, "%s", mapFilename) < 0){
    fprintf(stdout, "invalid file name.\n");
    mem_free(pathname);
    return NULL;
  }
  
  //try open file
  if ((fp = fopen(pathname, "r")) == NULL) {
    fprintf(stdout, "invalid file name\n");
    mem_free(pathname);
    return NULL;
  }

  int ncol = 0;
  int nrow = 0;

  nrow = file_numLines(fp); //num of lines in the map file
  char* firstline = file_readLine(fp); 
  ncol = strlen(firstline); //num of characters in each line
  fclose(fp);
  free(firstline);

  grid_t* grid = grid_new(nrow, ncol);
  
  //open file again to load map
  if ((fp = fopen(pathname, "r")) == NULL) {
    mem_free(pathname);
    return NULL;
  }
  
  //for each character in the file, store in grid
  for(int r = 0; r < nrow; r++){
    for(int c = 0; c < ncol; c++){
      char ch = fgetc(fp);
      if(ch == '\n'){
        ch = fgetc(fp); //skip new line character
      }
      grid_update(grid, r, c, ch);      
    }
  }    
  
  fclose(fp);
  mem_free(pathname);

  return grid;
}

/******************** grid_nrow ************************/
/* returns number of row for the given grid
 * returns -1 if grid is NULL
 */
int
grid_nrow (grid_t* grid)
{
  if(grid == NULL){
    return -1;
  }
  return grid->nrow;
}

/********************* grid_ncol ***************************/
/* returns number of col for the given grid
 * returns -1 if grid is NULL
 */
int
grid_ncol (grid_t* grid) 
{  
  if(grid == NULL){
    return -1;
  }
  return grid->ncol;
}

/********************* grid_getchar ************************/
/* returns a character at the given row and col of the grid
 * returns '^' if NULL (or I guess any character that we don't use in the game)
 */
char
grid_getchar (grid_t* grid, int r, int c) 
{
  if(grid == NULL){
    return '^';
  }
  if(r >= 0 && r < grid->nrow && c >= 0 && c < grid->ncol){
    return *(grid->map + r*(grid->ncol) + c);
  }
  return '^';
}

/********************** grid_update ************************/
/* Updates the character at the given row and col.
 * Does nothing if grid is NULL, or if row and col is out of bounds.
 */
void 
grid_update(grid_t* grid, int r, int c, char ch)
{
  if(grid != NULL){
    int nrow = grid->nrow;
    int ncol = grid->ncol;

    if (r >= 0 && r < nrow && c >= 0 && c < ncol) { 
        *(grid->map + r*ncol + c) = ch;
    }
  }
}

/******************** grid_isBlockable *******************/
/* returns true if the character at given row and col is a valid room space.
 * returns false otherwise.
 * the caller should not pass a NULL grid
 */
static bool 
grid_isBlockable(grid_t* grid, int r, int c)
{
  return grid_getchar(grid, r, c) != '.' && grid_getchar(grid, r, c) != '*' && !grid_isPlayer(grid, r, c);
}

/******************** grid_isEmptyRoomSpot *******************/
/* returns true if the character at given row and col is an empty room space.
 * returns false otherwise.
 * the caller should not pass a NULL grid
 */
bool 
grid_isEmptyRoomSpot(grid_t* grid, int r, int c)
{
  return grid_getchar(grid, r, c) == '.';
}

/******************** grid_isPlayer *******************/
/* returns true if the character at given row and col is an empty room space with player.
 * returns false otherwise.
 * the caller should not pass a NULL grid
 */
bool 
grid_isPlayer(grid_t* grid, int r, int c)
{
  return isalpha(grid_getchar(grid, r, c)) != 0 || grid_getchar(grid, r, c) == '@';
}

/********************* grid_isGold ***********************/
/* returns true if the character at given row and col is gold
 * returns false if not gold
 * the caller should not pass a NULL grid
 */
bool
grid_isGold(grid_t* grid, int r, int c)
{   
  return grid_getchar(grid, r, c) == '*';
}

/********************* grid_isBoundary ***********************/
/* returns true if the character at given row and col is boundary
 * returns false if not boundary
 * the caller should not pass a NULL grid
 */
bool
grid_isBoundary(grid_t* grid, int r, int c)
{
  return grid_getchar(grid, r, c) == '|' || grid_getchar(grid, r, c) == '-' || grid_getchar(grid, r, c) == '+';
}

/********************* grid_isRock ***********************/
/* returns true if the character at given row and col is empty ' '
 * returns false if otherwise
 * the caller should not pass a NULL grid
 */
bool
grid_isRock(grid_t* grid, int r, int c)
{
  return grid_getchar(grid, r, c) == ' ';
}

/********************* grid_canMoveTo ***********************/
/* returns true if the character at given row and col is '.', 'A-Z', '*', '#' 
 * returns false if otherwise
 * the caller should not pass a NULL grid
 */
bool
grid_canMoveTo(grid_t* grid, int r, int c)
{
  return !grid_isBoundary(grid, r, c)&&!grid_isRock(grid, r, c) && grid_getchar(grid, r, c) != '^';
}

/********************* grid_setVisibility ***********************/
/* updates the "known" grid based on the player's location (pr,pc)
 * Uses helper function grid_isVisible
 */
void
grid_setVisibility(grid_t* master, grid_t* raw, grid_t* known, int pr, int pc){
  if (master != NULL || raw != NULL || known != NULL) {   
    grid_clean(raw, known); //clean grid of player and gold information

    //update "known" grid according to master
    for(int r = 0; r < master->nrow; r++){
      for(int c = 0; c < master->ncol; c++){
        if(!grid_isRock(master, r, c)){ //ignore ' '
          if(grid_isVisible(master, pr, pc, r, c)){ //if point in map is visible to the player
            grid_update(known, r, c, grid_getchar(master, r, c)); //update known
          }
        }
      }
    }
    grid_update(known, pr, pc, '@');
  }
}

/********************* grid_isVisible ***********************/
/* Returns true if point r,c is visible at pr,pc on the master grid.
 * Returns false otherwise
 */
static
bool
grid_isVisible(grid_t* master, int pr, int pc, int r, int c)
{ 
  //r2 is greater than r1, c2 is greater than c1
  int r1; int c1;
  int r2; int c2;
  
  //Find out whether player's row or column is greater than those of given point, assign accordingly.
  if(pr > r){
    r1 = r;
    r2 = pr;
  }
  else{
    r1 = pr;
    r2 = r;
  }
  if(pc > c){
    c1 = c;
    c2 = pc;
  }
  else{
    c1 = pc;
    c2 = c;
  }
  
  //Find difference in row and col between pr,pc and r,c
  int drow = r - pr; 
  int dcol = c - pc;
  
  if(drow == 0){ //if the two points are in the same row
    for(int j = c1 + 1; j < c2; j++){ //for each column
      if(grid_isBlockable(master, pr, j)){ //find if there is any obstacle
        return false;
      }
    }
  }

  if(dcol == 0){ //if the two points are in the same col
    for(int i = r1 + 1; i < r2; i++){ //for each row
      if(grid_isBlockable(master, i, pc)){ //find if there is any obstacle
        return false;
      }
    }
  }

  for(int i = r1 + 1; i < r2; i++){ //for each row between pr and r (not necessarily in the order of pr->r)
      
    float ic = (float)(i - pr) * dcol / drow + pc; //find col value for row i of the line segment (may not be integer)

    if(ic == (int)ic){ //if the line segment intersects a gridpoint exactly
      if(grid_isBlockable(master, i, (int)ic)){ 
        return false;
      }
    }

    else{ //if the line segment intersects between two gridpoints
      if(grid_isBlockable(master, i, (int)ic) && grid_isBlockable(master, i, (int)ic + 1)){
        return false;
      }
    }
  }

  for(int i = c1 + 1; i < c2; i++){ //for each col between pc and c (not necessarily in the order of pc -> c)

    float ir = (float)(i - pc) * drow / dcol + pr; //find row value for col i of the line segment (may not be integer)

    if(ir == (int)ir){ //if the line segment intersects gridpoint exactly
      if(grid_isBlockable(master, (int)ir, i)){  
        return false;
      }
    }

    else{ //if the line segment intersects between two gridpoints 
      if(grid_isBlockable(master, (int)ir, i) && grid_isBlockable(master, (int)ir + 1, i)){
        return false;
      }
    }
  }
  return true;
}

/********************** grid_toString *************************/
/* return a string representing the grid.
 * returns NUll if grid is NULL.
 * the caller is reponsible for freeing the returned string.
 */
char*
grid_toString(grid_t* grid)
{
  if(grid == NULL){
    return NULL;
  }

  int nrow = grid->nrow;
  int ncol = grid->ncol;
  int size = nrow * ncol + nrow; //size for the resulting string. include newline characters.
  char* result = mem_malloc_assert(size + 1, "out of memory");
  
  for(int r = 0; r < nrow; r++){
    for(int c = 0; c < ncol; c++){
        int index = ncol*r + r + c;
        result[index] = grid_getchar(grid, r, c); //append each character
    }
    result[r*ncol + r + ncol] = '\n'; //newline after each row
  }
  result[size] = '\0'; //terminating character at the end of the string
   
  return result;
}


/********************** grid_clean *************************/
/* cleans the "known" grid based on the raw grid 
 * Used at the bgeinning of grid_setVisibility
 */
void
grid_clean(grid_t* raw, grid_t* known){
  
  if(raw != NULL && known != NULL){
    //reset gold and player in the "known" grid
    for(int r = 0; r < known->nrow; r++){
      for(int c = 0; c <known->ncol; c++){
        if(grid_isGold(known, r, c) || grid_isPlayer(known, r, c)){
          grid_update(known, r, c, grid_getchar(raw, r, c)); 
        }
      }
    }
  }
}


/********************** grid_delete *************************/
/* free non NULL grid
 */
void
grid_delete(grid_t* grid)
{
  if(grid != NULL){
    free(grid->map);
    free(grid);
  }
}
