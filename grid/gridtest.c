/* Unit test for the grid module
 * 
 * test functions from grid module with main.txt
 *
 * hemlock, May 2021
 */

//include files
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "grid.h"

int
main(const int argc, char* argv[])
{
  char* pathname = "../maps/main.txt"; //map to test with
  int nrow = 21; //expected nrow for the given map
  int ncol = 79; //expected ncol for the given map
    
  //TEST GRID_LOAD
  fprintf(stdout, "test grid_load.\n");

  grid_t* grid;
  fprintf(stdout, "load with invalid file name: ");
  grid = grid_load("invalid file name"); //invalid path name
  
  if(grid != NULL){
    fprintf(stdout, "grid_load should return NULL for invalid path.\n");
    exit(1);  
  } 
  
  fprintf(stdout, "load %s\n", pathname);
  grid = grid_load(pathname); //valid path name
  
  if(grid == NULL){
    fprintf(stdout, "grid_load failed.\n");
    exit(1);   
  }
  
  //TEST GRID_TOSTRING
  fprintf(stdout, "\ntest grid_toString.\n");
  char* gridS = grid_toString(grid);
  fprintf(stdout, "%s\n", gridS);
  free(gridS);
  
  //TEST GRID_NCOL, GRID_NROW
  fprintf(stdout, "\ntest grid_nrow, grid_ncol.\n");
  fprintf(stdout, "the row for %s should be %d: %d\n", pathname, nrow, grid_nrow(grid));
  if(nrow != grid_nrow(grid)){
    exit(1); //exit if nrow is not the expected number
  }

  fprintf(stdout, "the col for %s should be %d: %d\n", pathname, ncol, grid_ncol(grid));
  if(ncol != grid_ncol(grid)){ //expect if ncol is not the expected number
    exit(1);
  }
  
  /* TEST GETTERS AND SETTERS
   * test getters and setters
   * the boolean functions (grid_isGold, grid_isPlayer, etc) are wrapper functions for grid_getchar 
   * on various characters. 
   */
  fprintf(stdout, "\ntest grid_getchar and grid_update.\n");

  fprintf(stdout, "test with invalid row and col.\n"); //out of bounds nrow and ncol
  
  grid_update(grid, -1, -1, '*'); //should do nothing
  
  if(grid_getchar(grid, -1, -1) != '^'){
    fprintf(stdout, "grid_getchar failed for invalid row and col.\n");
    exit(1);
  }
  
  fprintf(stdout, "test with valid row and col.\n");
  
  //(0,0) should be initially ' '
  if(!grid_isRock(grid, 0, 0)){
    fprintf(stdout, "grid_isRock failed.\n");
    exit(1);
  } 
  
  //update (0,0) to gold, check grid_isGold
  grid_update(grid, 0, 0, '*');
  if(!grid_isGold(grid, 0, 0)){
    fprintf(stdout, "grid_isGold failed.\n");
    exit(1);
  }
  
  if(!grid_canMoveTo(grid, 0, 0)){
    fprintf(stdout, "grid_canMoveTo failed.\n");
    exit(1);
  }

  //update (0,0) to player, check grid_isPlayer
  grid_update(grid, 0, 0, 'P');
  if(!grid_isPlayer(grid, 0, 0)){
    fprintf(stdout, "grid_isPlayer failed.\n");
    exit(1);
  }
  
  if(!grid_canMoveTo(grid, 0, 0)){
    fprintf(stdout, "grid_canMoveTo failed.\n");
    exit(1);
  }

  //update (0,0) to '.', check grid_isEmptyRoomSpot, grid_canMoveTo
  grid_update(grid, 0, 0, '.');
  if(!grid_isEmptyRoomSpot(grid, 0, 0)){
    fprintf(stdout, "grid_isEmptyRoomSpot failed.\n");
    exit(1);
  }

  if(!grid_canMoveTo(grid, 0, 0)){
    fprintf(stdout, "grid_canMoveTo failed.\n");
    exit(1);
  }
  
  //update (0,0) to boundary, check grid_isBoundary, grid_canMoveTo
  grid_update(grid, 0, 0, '+');
  if(!grid_isBoundary(grid, 0, 0)){
    fprintf(stdout, "grid_update failed.\n");
    exit(1);
  }

  if(grid_canMoveTo(grid, 0, 0)){
    fprintf(stdout, "grid_canMoveTo failed.\n");
    exit(1);
  }

  //TEST VISIBILITY
  fprintf(stdout, "\ntest visbility.\n");
  
  grid_t* visible = grid_new(nrow, ncol);
  
  //manually plot points to check visibility for each location
  grid_setVisibility(grid, grid, visible, 19, 6); //update the visible grid
  grid_update(visible, 19, 6, '@'); //mark the position of the player
  char* visible1 = grid_toString(visible); //convert visible to string and print
  fprintf(stdout, "%s\n", visible1);
  free(visible1);
  
  //move to new point
  grid_clean(grid, visible);
  grid_setVisibility(grid, grid, visible, 14, 6);
  grid_update(visible, 14, 6, '@');
  char* visible2 = grid_toString(visible);
  fprintf(stdout, "%s\n", visible2);
  free(visible2);
  
  //move to new point
  grid_clean(grid, visible);
  grid_setVisibility(grid, grid, visible, 14, 30);
  grid_update(visible, 14, 30, '@');
  char* visible3 = grid_toString(visible);
  fprintf(stdout, "%s\n", visible3);
  free(visible3);
  
  //move to new point
  grid_clean(grid, visible);
  grid_setVisibility(grid, grid, visible, 16, 39);
  grid_update(visible, 16, 39, '@');
  char* visible4 = grid_toString(visible);
  fprintf(stdout, "%s\n", visible4);
  free(visible4);
  
  //move to new point (enter passage)
  grid_clean(grid, visible);
  grid_setVisibility(grid, grid, visible, 13, 52);
  grid_update(visible, 13, 52, '@');
  char* visible5 = grid_toString(visible);
  fprintf(stdout, "%s\n", visible5);
  free(visible5);
  
  //move to new point (enter passage)
  grid_clean(grid, visible);
  grid_setVisibility(grid, grid, visible, 13, 53);
  grid_update(visible, 13, 53, '@');
  char* visible6 = grid_toString(visible);
  fprintf(stdout, "%s\n", visible6);
  free(visible6);
  
  //delete 
  grid_delete(visible);
  grid_delete(grid);

  return 0;
}
