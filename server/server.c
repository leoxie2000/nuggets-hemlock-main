/*
 * server.c - Nuggest's server
 *
 * Usage: ./server map.txt [seed]
 *
 * Team - Hemlock, May 2021
 *
 */

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "message.h"
#include "log.h"
#include "grid.h"

/***************************************************************************************/
#define MaxNameLength 50   // max number of chars in playerName
#define MaxPlayers 26      // maximum number of players
#define GoldTotal 250      // amount of gold in the game
#define GoldMinNumPiles 10 // minimum number of gold piles
#define GoldMaxNumPiles 30 // maximum number of gold piles

/******************************* player struct *****************************************/
typedef struct player {
  addr_t IP;                       // IP address
  char realName[MaxNameLength + 1];
  char alias;                       // letter assigned
  int gold;                        // gold in purse
  int justCollected;
  int row;                         // row
  int col;                         // column 
  grid_t* seenGrid;
} player_t;



/******************************* game struct *****************************************/
struct game
{
  // grid
  grid_t* masterGrid;
  grid_t* rawGrid;
  int GridRow;
  int GridCol;

  // gold
  int GoldNumPilesLeft;
  int goldCollected;             // total gold collected so far
  int goldLeft;                  // gold left
    
  // player
  player_t* players[MaxPlayers]; //array of player struct
  int numPlayer; 
    
  // spectator
  addr_t spectatorIP;

  // seed
  int seed;
};


/******************************* function declaration ********************************/
static void parseArgs(const int argc, char* argv[]);

static void server_drop_gold(void);

static void server_drop_player(player_t* player);

static void server_update_all_clients(void);

bool handleMessage(void* arg, const addr_t from, const char* message);

bool handlePlay(void* arg, const addr_t from, const char* name);

bool handleSPECTATE(void* arg, const addr_t from);

bool handleQUIT(addr_t IP);

bool handleKEY(void* arg, const addr_t from, const char* key);

bool player_move(player_t* player, int new_row, int new_col);

static void pickup_gold(player_t* player);

bool helper_nameIsEmpty(const char* name);

static void game_over(void);

/*************************************************************************************/  
struct game GAME;

/************************************** main *****************************************/
int
main(const int argc, char* argv[]) {

  GAME.numPlayer = 0;

  GAME.spectatorIP = message_noAddr();

  // parse the arguments
  // 1. verify the argument
  // 2. initialize the random number generator srand
  // 3. load the map
  parseArgs(argc, argv);

  // drop the gold
  server_drop_gold();

  // initialize error log  
  //log using log_c, log_v, log_s, log_e
  log_init(stderr);

  // initalize the network and announce the port number
  int serverPort = message_init(stdout);

  if (serverPort == 0){
    log_e("Error: serverPort initialization failed\n");
    exit(-6);
  } else {
    printf("Ready to play, waiting at port %d", serverPort);
  }

  // start the message loop
  // return type bool
  // true for success return (true)
  // false on error

  bool status = message_loop(NULL, 0, NULL, NULL, handleMessage);
  printf("%d",status);
  
  // print summery table and send back final quit message
  game_over();

  message_done();

  log_done();
} 

/******************************** parseArgs ***********************************/
static void
parseArgs(const int argc, char* argv[])
{
  if (argc == 2) {
    char* map = argv[1];
    GAME.masterGrid = grid_load(map);
    GAME.rawGrid = grid_load(map);

    if ((GAME.masterGrid) == NULL) {
      fprintf(stderr, "Error: fail to load map: %s \n", map);
      exit(-1);
    }

    GAME.GridCol = grid_ncol(GAME.masterGrid);
    GAME.GridRow = grid_nrow(GAME.masterGrid);
    srand(getpid());
  }
  else if (argc == 3) {
    char* map = argv[1];
    GAME.masterGrid = grid_load(map);
    GAME.rawGrid = grid_load(map);
    if ((GAME.masterGrid) == NULL) {
      fprintf(stderr, "Error: fail to load map: %s \n", map);
      exit(-1);
    }

    int seed = atoi(argv[2]);
  
    if (seed <= 0) {
      fprintf(stderr, "Error: Seed should be a positive integer\n");
      exit(-2);
    }

    GAME.GridCol = grid_ncol(GAME.masterGrid);
    GAME.GridRow = grid_nrow(GAME.masterGrid);
    srand(GAME.seed);

  } else {
    fprintf(stderr, "usage: ./server map.txt [seed]");
    exit(-3);
  }
}

/******************************* server_drop_gold ************************************/
static void
server_drop_gold(void)
{
  GAME.GoldNumPilesLeft = rand() % (GoldMaxNumPiles - GoldMinNumPiles);
  GAME.GoldNumPilesLeft = GAME.GoldNumPilesLeft + GoldMinNumPiles;

  GAME.goldCollected = 0;
  GAME.goldLeft = GoldTotal;

  for (int i = 0; i < GAME.GoldNumPilesLeft; i++) {
    int row = rand() % GAME.GridRow;
    int col = rand() % GAME.GridCol;
    while (!grid_isEmptyRoomSpot(GAME.masterGrid, row, col)) {
      row = rand() % GAME.GridRow;
      col = rand() % GAME.GridCol;
    }
    grid_update(GAME.masterGrid, row, col, '*');
  }
}

/****************************** server_drop_player *********************************/
static void
server_drop_player(player_t* player)
{
  int row = rand() % GAME.GridRow;
  int col = rand() % GAME.GridCol;

  while (!grid_isEmptyRoomSpot(GAME.masterGrid, row, col)) {
    row = rand() % GAME.GridRow;
    col = rand() % GAME.GridCol;
  }

  player->row = row;
  player->col = col;

}

/***************************** server_update_all_clients **************************/
static void
server_update_all_clients(void) {

  int size = (GAME.GridRow) * (GAME.GridCol) + 500;
  char message[size];

  if (message_isAddr(GAME.spectatorIP)) {
    // GOLD n p r
    sprintf(message, "GOLD %d %d %d", 0, 0, GAME.goldLeft);
    message_send(GAME.spectatorIP, message);
    // DISPLAY/nstring
    char* disp_string = grid_toString(GAME.masterGrid);
    sprintf(message, "DISPLAY\n%s", disp_string);
    message_send(GAME.spectatorIP, message);
    free(disp_string);
  }

  for (int i = 0; i < GAME.numPlayer; i++) {
    
    player_t* player = GAME.players[i];
    
    /// GOLD n p r 
    sprintf(message, "GOLD %d %d %d", player->justCollected, player->gold, GAME.goldLeft);
    message_send(player->IP, message);
    player->justCollected = 0;

    // DISPLAY/nstring
    grid_setVisibility(GAME.masterGrid, GAME.rawGrid, player->seenGrid, player->row, player->col);
    char* disp_string = grid_toString(player->seenGrid);
    sprintf(message, "DISPLAY\n%s", disp_string);
    message_send(player->IP, message);
    free(disp_string);
  }
    
}

/********************************** handleMessage **********************************/
bool handleMessage(void* arg, const addr_t from, const char* message)
{
  // copy the message into local string
  char msg[strlen(message)];
  strcpy(msg, message);

  if (strncmp(msg, "PLAY ", strlen("PLAY ")) == 0) {
    char *name = msg + strlen("PLAY ");
    return handlePlay(arg, from, name);
  }
  else if (strncmp(msg, "SPECTATE", strlen("SPECTATE")) == 0) {
    return handleSPECTATE(arg, from);
  }
  else if (strncmp(msg, "KEY ", strlen("KEY ")) == 0) {
    char *key = msg + strlen("KEY ");
    return handleKEY(arg, from, key);
  }
  else {
    // Invalid message
    log_e("Invalid message"); 
    //continur
    return false;
  }
}

/********************************* handlePlay ***************************************/
bool handlePlay(void* arg, const addr_t from, const char* name) {
  if (GAME.numPlayer == MaxPlayers) {
    message_send(from, "QUIT Game is full: no more players can join.");
  }
  else if (helper_nameIsEmpty(name)) {
    message_send(from, "QUIT Sorry: you must provide player's name.");
  }
  else {
    // new player
    player_t* player = malloc(sizeof(player_t));
    if (player == NULL) {
      //out of mem
    } else {
      // player IP
      player->IP = from;
      // player realname
      for (int i = 0; i < strlen(name); i++) {
        if (i < MaxNameLength) {
          if (isgraph(name[i]) == false && isblank(name[i] == false)) {
            (player->realName)[i] = '_';
            (player->realName)[i+1] = '\0';
          } else {
            (player->realName)[i] = name[i];
            (player->realName)[i+1] = '\0';
          }
        }
      }
    
      // player alias
      player->alias = 'A' + GAME.numPlayer;
    
      // player gold
      player->gold = 0;
      player->justCollected = 0;

      // player location
      server_drop_player(player);
    
      // player seenGrid
      player->seenGrid = grid_new(GAME.GridRow, GAME.GridCol);
        
      // add new player into the players list
      GAME.players[GAME.numPlayer++] = player;
      grid_update(GAME.masterGrid, player->row, player->col, player->alias);

      // Message
      char message[100];
      // Message 1 OK
      sprintf(message, "OK %c", player->alias);
      message_send(from, message);
      // Message 2 GRID
      sprintf(message, "GRID %d %d", GAME.GridRow, GAME.GridCol);
      message_send(from, message);

      // update all clients
      server_update_all_clients();
    }
  }
  // game continue
  return false;
}

/******************************* handleSPECTATE ****************************/
bool handleSPECTATE(void* arg, const addr_t from) {

  if (message_isAddr(GAME.spectatorIP)) {
    message_send(GAME.spectatorIP, "QUIT You have been replaced by a new spectator.");
  }

  GAME.spectatorIP = from;

  // Message spectator
  char message[100];
  sprintf(message, "GRID %d %d", GAME.GridRow, GAME.GridCol);
  message_send(from, message);
  
  // send update to all clients
  server_update_all_clients();

  // game continue
  return false;
}

/******************************** handleQUIT *******************************/
bool handleQUIT(addr_t IP)
{
  if (message_eqAddr(GAME.spectatorIP, IP)) {
    message_send(GAME.spectatorIP, "QUIT Thanks for watching!");
  }
  
  for (int i = 0; i < GAME.numPlayer; i++) {
    player_t* current = GAME.players[i];
    if (message_eqAddr(IP, current->IP)) {
      message_send(IP, "QUIT Thanks for playing!");
     
      // replace the masterGrid's player symbol
      char raw_char = grid_getchar(GAME.rawGrid, GAME.players[i]->row, GAME.players[i]->col);
      grid_update(GAME.masterGrid, GAME.players[i]->row, GAME.players[i]->col, raw_char);
    
    }
  }

/* Make the prorgam easier for other ppl
 
      // delete the player
      grid_delete(GAME.players[i]->seenGrid);
      free(GAME.players[i]);
      for (int j = i + 1; j < GAME.numPlayer; j++) {
        GAME.players[j-1] = GAME.players[j];
      }
      GAME.numPlayer--;
    }
  }
*/

  // send update to all clients and game continue
  server_update_all_clients();
  return false;
}

/********************************* handleKEY *******************************/
bool handleKEY(void* arg, const addr_t from, const char* key)
{
  char KEY = *key;
  player_t* player = NULL;
  
  // Let the player quit the game; game continue
  if (KEY == 'Q') {
    handleQUIT(from);
    return false;
  }

  // figure out which player sent the message
  for (int i = 0; i < GAME.numPlayer; i++) {
    player_t* current = GAME.players[i];
    if (message_eqAddr(from, current->IP)) {
      player = current;
    }
  }

  if (player == NULL) {
   //error
   log_e("Player must join first before playing");
   return false;
  }

  int row = player->row;
  int col = player->col;
    
  switch(KEY)
  {
    case 'h':
      //left
      player_move(player, row, col-1);
      break;

    case 'H': 
      //left max
      while(!player_move(player, row, col-1)) {
        col--;
      }
      break;
              
    case 'l':
      //right
      player_move(player, row, col+1);
      break;
  
    case 'L': 
      //right max
      while(!player_move(player, row, col+1)) {
        col++;
      }
      break;
    
    case 'j':
      //down
      player_move(player, row+1, col);
      break;
 
    case 'J':
      //down max
      while(!player_move(player, row+1, col)) {
        row++;
      }
      break;   
    
    case 'k':
      //up
      player_move(player, row-1, col);
      break;
    
    case 'K':
      //up max
      while(!player_move(player, row-1, col)) {
        row--;
      }
      break;

    case 'y':
      //up left
      player_move(player, row-1, col-1);
      break;
   
    case 'Y':
      //up left max
      while(!player_move(player, row-1, col-1)) {
        row--;
        col--;
      }
      break;
   
     case 'u':
      //up right
      player_move(player, row-1, col+1);
      break;

     case 'U':
      //up right max
      while(!player_move(player, row-1, col+1)) {
        row--;
        col++;
      }
      break;

     case 'b':
      //down left
      player_move(player, row+1, col-1);
      break;
  
     case 'B':
      //down left max
      while(!player_move(player, row+1, col-1)) {
        row++;
        col--;
      }
      break;
      
     case 'n':
      //down right
      player_move(player, row+1, col+1);
      break;

     case 'N':
      //down right max
      while(!player_move(player, row+1, col+1)) {
        row++;
        col++;
      }
      break;
     
     default:
      //invalid key
      log_c("Invalid Key : %c", KEY);
      char messageUnknown[100];
      snprintf(messageUnknown, 100, "ERROR Unknown Keystroke: %c", KEY);
      message_send(player->IP, messageUnknown);
  }

  // check the # of gold left to determine whether to end the game
  if (GAME.GoldNumPilesLeft == 0) {
    // only time to quit normally
    // No gold left
    return true;
  } else {
    // game continue
    return false;
  }
}

/******************************** player_move ***********************************/
bool
player_move(player_t* player, int new_row, int new_col)
{
  int old_row = player->row;
  int old_col = player->col;

  if (grid_canMoveTo(GAME.masterGrid, new_row, new_col)) {
    // ok to move
    if (grid_isPlayer(GAME.masterGrid, new_row, new_col)) {
      // switch location
      for (int i = 0; i < GAME.numPlayer; i++) {
        player_t* another_player = GAME.players[i];
        if (another_player->row == new_row && another_player->col == new_col) {
          another_player->row = old_row;
          another_player->col = old_col;
          grid_update(GAME.masterGrid, another_player->row, another_player->col, another_player->alias);
          
          player->row = new_row;
          player->col = new_col;
          grid_update(GAME.masterGrid, player->row, player->col, player->alias);
          server_update_all_clients();
          return false;
        }
      }
    }
    else if (grid_isGold(GAME.masterGrid, new_row, new_col)) {
      player->row = new_row;
      player->col = new_col;
      // find the original char, it actually must be .
      char raw_char = grid_getchar(GAME.rawGrid, old_row, old_col);
      grid_update(GAME.masterGrid, old_row, old_col, raw_char);
      grid_update(GAME.masterGrid, player->row, player->col, player->alias);
      pickup_gold(player);
      server_update_all_clients();
    }
    else {
      // empty room spot or passage
      player->row = new_row;
      player->col = new_col;
      // find the original char, it can be . or #
      char raw_char = grid_getchar(GAME.rawGrid, old_row, old_col);
      grid_update(GAME.masterGrid, old_row, old_col, raw_char);
      grid_update(GAME.masterGrid, player->row, player->col, player->alias);
      server_update_all_clients();
    }
    // may be able to move further
    return false;
  } else {
    // can not move
    return true;
  }
}

/********************************** pickup_gold ***********************************/
/* pick up the gold at the player's location 
 * 
 * return:
 *  true when no gold is left (indicate quit)
 *  false (do not quit) - reaminging gold for player
 *
 */ 
static void
pickup_gold(player_t* player)
{
  int minPerPile = 1;
  int maxPerPile = (GAME.goldLeft) - (GAME.GoldNumPilesLeft) + 1;
  int gold = 0;

  if (GAME.GoldNumPilesLeft != 1){
    gold = rand() % (maxPerPile - minPerPile + 1);
    gold = gold + minPerPile;
  } else {
    gold = GAME.goldLeft;
  }

  player->gold = player->gold + gold;
  player->justCollected = gold;

  GAME.goldCollected = GAME.goldCollected + gold; 
  GAME.goldLeft = GAME.goldLeft - gold;
  GAME.GoldNumPilesLeft--;
}

/******************************** helper_nameIsEmpty *******************************/
bool
helper_nameIsEmpty(const char* name) {
  for (int i = 0; i < strlen(name); i++) {
    if (!isspace(name[i])) {
      return false;
    }
  }
  return true;
}

/******************************* game_over ***************************************/
static void
game_over(void)
{

  char message[26000] = "QUIT GAME OVER:\n";

  for (int i = 0; i < GAME.numPlayer; i++) {
    player_t* player = GAME.players[i];
    char mc[2];
    mc[0] = player->alias;
    mc[1] = '\0';
    strcat(message, mc);
    int gold = player->gold;
    if (gold == 0) {
      strcat(message, "     0   ");
    }
    else if ((gold/10) == 0) {
      char mn[100];
      snprintf(mn, 100, "     %d   ", gold);
      strcat(message, mn);
    }
    else if ((gold/100) == 0) {
      char mn[100];
      snprintf(mn, 100, "    %d   ", gold);
      strcat(message, mn);
    }
    else if ((gold/1000) == 0) {
      char mn[100];
      snprintf(mn, 100, "   %d   ", gold);
      strcat(message, mn);
    }
    strcat(message, player->realName);
    strcat(message, "\n");
  }

  if (message_isAddr(GAME.spectatorIP)) {
    message_send(GAME.spectatorIP, message);
  }

  for (int i = 0; i < GAME.numPlayer; i++) {
    player_t* player = GAME.players[i];
    message_send(player->IP, message);
  }

  for (int i = 0; i < GAME.numPlayer; i++) {    
    player_t* player = GAME.players[i];
    grid_delete(player->seenGrid);
    free(player);
  }

  grid_delete(GAME.masterGrid);
  grid_delete(GAME.rawGrid);
}
