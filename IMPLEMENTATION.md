# CS50 nuggets Hemlock
## Implementation Spec

Here we focus on the core subset:

-  Data structures
-  Control flow: pseudo code for overall flow, and for each of the functions
-  Detailed function prototypes and their parameters
-  Error handling and recovery
-  Detailed teamwork distribution
-  Testing plan

## Data structures 

### Server
The server holds the game status and communicates with clients for gameplay.
Game struct contains all game elements including grid_t* and a array of players' pointers.
Player struct hold the information for the player.

Global constants:
```c
static const int MaxNameLength = 50;   // max number of chars in playerName
static const int MaxPlayers = 26;      // maximum number of players
static const int GoldTotal = 250;      // amount of gold in the game
static const int GoldMinNumPiles = 10; // minimum number of gold piles
static const int GoldMaxNumPiles = 30; // maximum number of gold piles
```
Global variable:
```c
static game_t GAME; //master game struct globally
```
Player struct:
```c
typedef player{
    addr_t IP;                       // IP address
    char realName[MaxNameLength];
    char alis;                       // letter assigned
    int gold;                        // gold in purse
    int justCollected;
    int row;                         // row
    int col;                         // column 
    grid_t* seenGrid;
} player_t;
```


Game struct:
```c
typedef game {
  // grid
  grid_t* masterGrid;
  gird_t* rawGrid;
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
}
```

### Grid

The `map` in the `grid` is a pointer to the first character in the `grid`. Memory is allocated for nrow * ncol characters.

Grid struct:
```c
typedef struct grid {
 char* map; //pointer to the character at (0,0)
 int nrow; 
 int ncol;
} grid_t;
```


### Player
The player is distinguished based on identity as player or spectator. Players can play game interactively and receive information about what they are allowed to see, and spectators receives information about all players and master game status.

gameInfo struct: Holds information about the player's identity, size of grid, and address of the server.

```c
typedef struct gameInfo{
    int GridRows;
    int GridCols;
    addr_t serverAddr;
    bool isPlayer;
} gameInfo_t;
```



## Control flow

The game is divided up in three major components: the server, player, and grid. The game is initialized with server, which clients can then join by running the player program. Server utilizes functions in the grid module to update game status and update players on the game progress.

### Server

The server implements all the game logic, and comnunicate with players through provided network protocal. The server start from command line inputs, parse the argument, initalize all the module, including load the map in and initialize all the local varibles. Then start the the message loop, processing each message by respective handle function, and send back the game information back to the clients.


#### main
Initialize the game, call parseArg, start message log, initalize network, and start message loop, print summery and quit message.

Pseudocode:
        
        Declare a game struct, initialize nesseary variable
        call parseArg
        call server_drop_gold
        start message log
        initialize network and annouce the port
        start message loop
        print summery and quit message


#### parseArgs:
This function parse the argument, load the map, initialize the random number generator

Pseudocode:

    if the number of argument is 2
        load the map into the master grid
        if the game is not valid
            quit the game
        intialize the random number generator
    if the number of argument is 3
        load the map into the master grid
        if the game is not valid
            quit the game
        if the seed number is negative
            quit the game
        intialize the random number generator with the seed
    else
       print an error message and quit non zero


#### server_drop_gold:
This function drops gold on the grid based on the specifications

Pseudocode:
        
        generate a random number for the number of goldpiles
        for each pile
            generate random location update the mastergrid
            while that location is a empty room spot
                generate a new random location
            update the master grid
            
            
#### server_update_all_clients:
This function send the gold info and each grid for every clients based on the specifications

Pseudocode:
        
        if there is a spectator
            send gold info 
            send the master grid
        iterate through every player
            send gold info 
            reset the just picked up gold to zero
            call set_visibility to get the visibilty grid and send


#### handleMessage:
This function handles incoming message and call respective handle function for each type of message

Pseudocode:

        if the message starts with play
            call handlePLAY
        if the message starts with SPECTATE
            call handleSPECTATE   
        if the message starts with KEY    
            call handleKEY
        else
            invalid input
            
#### handlePlay:
This function handles play message, add a new player to the game

Pseudocode:

        if the game is full
            send a quit message back to the player
        if the name is Empty
            send a quit message back
        otherwise
            create a new player
            copy the player name in to the real name
            create a alias and store in player struct
            initialize gold related varible
            drop the player on to the grid, update the player location and the master grid
            increment the number of players in the game
            message back required start game info, alias and size
            send update to all clients
            
        return true if anyfunction return true
        otherwise return false

#### handleSPECTATE:
This function handles spectate message, add or replace spectator

Pseudocode:

        if there exists one spectator
            send a quit message to the existing one
        store the spectator IP in to game struct (auto replace if needed)
        send a grid info message
        update all clients
        return false

#### handleQUIT:
This function handles quit for clients

Pseudocode:

        if the clients is spectator
            send spectator quit message
        interate through the players list
            if the player has the same IP as the one trying to quit
                send a thanks for playing message to that player
                delete that player
                decrement the number of player
        return false         

#### handleKEY:
This function handles key for clients

Pseudocode:

        if the key is Q
            call handleQUIT with the IP
            return flase to continue the loop
        otherwise
            depending on the key
                for lower case
                    call player_move with new location
                for Upper case
                    call player_move with new location until return true
        if no gold left
            return true
        else
            return false
        


#### player_move:
This function handles actual move for the players

Pseudocode:

        if the new location is allowed to move to 
            if the new location is a player
                switch positions
                update grid
                call update_all_clinets
                return false
            if the new location is gold
                update the new location of the player
                update grid 
                call pickup_gold to pick up gold
                send update to all clients
            else (location is just empty room spot)
                update the new location of the player
                replace the old location with the original character
                update grid
                send update to all clients
                return false
        else
            return true
            


#### pickup_gold:
This function handles player pick up gold

Pseudocode:
    
        if more than one gold pile left
            generate a random number in the valid range
        if only one gold pile
            take all the gold
        
        update all the related gold info for the player and in the game struct
        


### Player

The client is implemented in `player.c` with two types of behaviors: the player and the spectator. As a player, one receives information about display specific to itself, and is able to move around to collect gold by using proper keystrokes. As a spectator one cannot play the game but is able to receive information about all players and all the game status.


Pseudocode for `main`:

        calls parseArgs;
        opens file for logging and validates it;
        initialize message module;
        if client is player:
            calls message_loop with handleMessage and handleInput
        if client is spectator:
            calls meesage_loop with handleMessage only;
         
Pseudocode for `game_init`:
     
        initialize the screen;
        accept keystrokes immediately;
        prevent echoing characters to screen
        initialize color
        get the current boundaries of the screen
        while current boundary < grid boundary:
            prompt user to enlarge window;
            update current window size;
    
     
Pseudocode for `parseArgs`:

        verifies parameters
        determine player type based on argc and set `isPlayer`
        call message_setAddr on current host name and port, get server address
     
Pseudocode for `handleInput`:

        get a character key 'c'
        if key is 'h': send message to server "KEY h"
        if key is 'l': send message to server "KEY l"
        if key is 'j': send message to server "KEY j"
        if key is 'k': send message to server "KEY k"
        if key is 'y': send message to server "KEY y"
        if key is 'u': send message to server "KEY u"
        if key is 'b': send message to server "KEY b"
        if key is 'n': send message to server "KEY n"
        if key is 'Q': send message to server "KEY Q"
        otherwise send message to server "KEY 'c'"
     
     
     
Pseudocode for `handleMessage`:

        if message begins with "GRID ":
            call parse_GRID on the content
            return false
        if message begins with "GOLD ":
            call parse_GOLD on the content
            return false
        if message begins with "DISPLAY\n":
            simply prints display string on window
            return false
        if message begins with "ERROR ":
            prints and logs error
            return false
        if message begins with "QUIT ":
            prints quiting message
            return true
Pseudocode for `parse_GOLD`:

        parses message for gold just collected, gold collected total, and gold left
        prints message on screen for these info
     
Pseudocode for `parse_GRID`:

        parses the size of grid
        initialize constants GridRows and GridCols based on grid size;


### Grid

The grid module is implemented in `grid.c` with functions that allows the server to load, manipulate and delete the grid. The module defines a grid struct that contains a pointer to the character at _(0,0)_.

Pseudocode for `grid_new`:

        if row and column are non-negative
            Allocate memory for grid with given row and column 
            For every space in grid
                set each character as empty space
        return grid

Pseudocode for `grid_load`:

        if map file readable
            count number of rows and columns
        for each character in map
            if newline
                skip 
            update grid
        close file

Pseudocode for `grid_nrow`:

        if grid is not NULL
            return nrow in grid struct

Pseudocode for `grid_ncol`:

	    if grid is not NULL
            return ncol in grid struct

Pseudocode for `grid_getchar`:

	    if grid is NULL
            return unused character
        if nrow and ncol is within map boundary
            return character in the row, col space
    
Pseudocode for `grid_update`:

	    if grid is not NULL
            if nrow and ncol is within map boundary
                assign map space with given character 

Pseudocode for `grid_isBlockable`:

	    return true if grid_getchar is not '.' and '*' and player (A-Z, @)

Pseudocode for `grid_isEmptyRoomSpot`:

	    return true if grid_getchar is '.'
    
Pseudocode for `grid_isPlayer`:

	    return true if grid_getchar is 'A-Z' or '@'

Pseudocode for `grid_isGold`:
    
	    return true if grid_getchar is '*'
    
Pseudocode for `grid_isBoundary`:

	    return true if grid_getchar is '|' or '-' or '+'

Pseudocode for `grid_isRock`:

	    return true if grid_getChar is ' '
    
Pseudocode for `grid_canMoveTo`:

        returns true if the character at given row and col is '.', 'A-Z', '*', '#'
    
Pseudocode for `grid_setVisibility`:

	    clean known grid
        for each point in the master grid
            if the grid space is not rock
                check which points are visible with grid_isVisible
                update the known grid with what is visible
            update player's location
    
Pseudocode for `grid_isVisible`:

	    check whether player's row/column is greater than given point's row/col
        calculate difference in row and col between given point and player's location
    
        if two points are in same row
            for each column
                return false if there is any obstacle 
        if two points are in same column
            for each row 
                return false if there is any obstacle
    
        for each row between given point and player location
            calculate column from row number and gradient
                if column is integer value
                    return false if there is obstacle in that row, col space
                else
                    return false if there is obstacle in that row, col and row, col+1 space

        for each col between given point and player location
            calculate row from col number and gradient
                if row is integer value
                    return false if there is obstacle in that row, col space
                else
                    return false if there is obstacle in that row, col and row+1, col space
    
        return true

Pseudocode for `grid_toString`:

        calculate size with ncol and nrow, plus newline characters 
        allocate memory for result string
    
        for each row
            for each column
                insert char from grid_getchar into appropriate index
                add newline after each row
      
        null terminate string 
        return result string

Pseudocode for `grid_clean` :
    
        for each row in the known grid
            for each col in the known grid 
                if the char is player or gold
                    update it with the original character from the raw grid
    
Pseudocode for `grid_delete`:

        if grid is not NULL
            free map and grid

### Support

We leverage the modules of support, most notably `message` and `log`.
See that directory for module interfaces.


## Function prototypes

### Server

Detailed descriptions of each function's interface is provided in a paragraph in the form of function before the function in 

```c
static void parseArgs(const int argc, char* argv[]);
static void server_drop_gold(void);
static void server_drop_player(player_t* player);
static void server_update_all_clients(void);
bool handleMessage(void* arg, const add_t from, const char* message);
bool handlePlay(void* arg, const add_t from, const char* name);
bool handleSPECTATE(void* arg, const add_t from, const char* message);
bool handleQUIT(add_t IP);
bool handleKEY(void* arg, const add_t from, const char* key);
bool player_move(player_t* player, int new_row, int new_col);
static void pickup_gold(player_t* player);
```

### Grid

Detailed descriptions of each function's interface is provided as a paragraph comment prior to each function's declaration in `grid.h` and is not repeated here.

```c
typedef struct grid grid_t;
// initializes a grid sized nrow*ncol.
grid_t* grid_new(int nrow, ncol);
// loads a grid from a given filename.
grid_t* grid_load(char* mapFilename);
// returns the number of row for the grid.
int grid_nrow(grid_t* grid);
// returns the number of col for the grid.
int grid_ncol(grid_t* grid);
// returns a character at the given row and col of the grid.
char grid_getchar(grid_t* grid, int r, int c);
// updates the character at the given row and col
void grid_update(grid_t* grid, int r, int c, char ch);
// returns true if the character is a room space can block visibility.
static bool grid_isBlockable(grid_t* grid, int r, int c);
// returns true if the character is an empty room space ('.')
bool grid_isEmptyRoomSpot(grid_t* grid, int r, int c);
// returns true if the character at the given row and col is gold ('*'). 
bool grid_isGold(grid_t* grid, int r, int c);
// returns true is the character is A~Z or @.
bool grid_isPlayer(grid_t* grid, int r, int c);
// returns true if the character is boundary.
bool grid_isBoundary(grid_t* grid, int r, int c);
// returns true if the character at given row and col is ' '.
bool grid_isRock(grid_t* grid, int r, int c);
// returns true if the character at given row and col is '.', 'A-Z', '*', '#' 
bool grid_canMoveTo(grid_t* grid, int r, int c);
// updates the "known" grid based on the player's location (pr, pc).
void grid_setVisibility(grid_t* master, grid_t* raw, grid_t* known, int pr, int pc);
//returns true if point (r,c) is visible at point (pr,pc)
static bool grid_isVisible(grid_t* grid, int pr, int pc, int r, int c);
// Converts a grid to a string.
char* grid_toString(grid_t* grid);
//cleans the "known" grid and replace it with characters from the raw grid.
void grid_clean(grid_t* raw, grid_t* known);
// Deletes a grid.
void grid_delete(grid_t* grid);
```

### Player

```c
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void game_init(void);
static void parseArgs(const int argc, const char* argv[]);
static void parse_GRID(char* msg);
static void parse_GOLD(char* msg);
```


## Error handling and recovery

The command-line arguments are thoroughly checked before any data structures is created or work begins. Any issues result in a message printed to stdout for the user to see and a non-zero exit status. In such cases, we cannot guarantee that the memory will be freed. In addition, the player program uses the ncurses library which has internal memory leaks that we cannot guarantee that will be freed.

Out-of-memory errors are handled by a message printed to stderr and a non-zero exit status by utilizing the mem module. We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

Simple non-fatal errors such as the player sending wrong keystrokes will not result in the program to crash. Similarly, if the server sends false messages, the program will not crash either. Instead, both the server and the player will ignore these messages and just simply log it.

All code uses defensive-programming tactics to catch and exit, if a function receives bad parameters.

## Teamwork distribution

### Jeff

Jeff is responsible for implementing following functions:

- Implement the server functions `parseArgs`, `server_drop_gold`, `server_drop_player`, `server_update_all_clients`, `handleMessage`, `handlePlay`, `handleSPECTATE`, `handleQUIT`, `handleKEY`, `player_move`, `pickup_gold` by May 26th
- Further debugging of the server and completed server by May 29th

### Leo

Leo is responsible for implementing the following:

- Implement the initial server `main`, `server_drop_gold` and `server_drop_player` by May 22nd. (Will be revised further by Jeff)
- Implement the `player.c` program which contains the following functions by May 26th:
    - `handleMessage`, `handleInput`, `game_init(void)`, `parseArgs`, `parse_GRID`, and `parse_GOLD`
- Finish testing of `player.c` by May 28th and help debug server starting May 28th.



### Tay
Tay is responsible for implementing the following:

- Implement `grid_setVisibility` and `grid_isVisible` by May 23rd.
- Write `grid.h` by May 23rd.
- Write `gridtest.c` and test grid module by May 26th.
- Write `README.md` for `grid` module by May 26th.
- Create a new map and upload it in `maps/contrib` directory by May 26th.

### Tim

Tim is responsible for implementing the following:

- Implement `grid_new`, `grid_load`, `grid_nrow`, `grid_ncol`, `grid_getchar`, `grid_update`, `grid_isEmptyRoomSpor`,  `grid_isBlockable`, `grid_isGold`, `grid_isPlayer`, `grid_isBoundary`, `grid_isRock`, `grid_toString`, and  `grid_delete` by May 21st.
- Write `Makefile` for `grid.c` by May 21st.

### Together

- test the program together by May 30th.
- prepare for presentation

## Testing plan

Here is an implementation-specific testing plan.

### Unit testing
There are three units: `server`, `client` and the `grid`. Testing for `server` and `client` will be covered below in integration/system testing. 

For `grid`, we will implement a small driver `gridtest.c` to test all the functions used in the grid module. We will manually update points in `grid` by using `grid_update` for testing purposes. 

1. test `grid_load` with invalid map file name.
2. test all setter and getter functions with out of bounds row and col value.
3. test all functions with NULL grid.
4. use `grid_update` to check boolean functions (`grid_isPlayer`, `grid_isGold`, etc)
5. use `grid_update` to check visibility. Manually update player's location and check visibility for each spot.
6. Run *valgrind* on `grid` to ensure no memory leaks or errors.


### Integration/system testing

*Integration testing*.  The nuggets game, as a complete program, will be mainly tested with the protocals below.
1. Test `server` with various invalid arguments.
	1. no arguments
	2. invalid seed (not integer)
	3. three or more arguments
 
2. Test `player` with various invalid arguments.
    1. no arguments
    2. one argument
    3. four or more arguments

3. Test `player` with valid arguments and the following:
    1. test with normal length player name
    2. test with extra long player name
    3. test without player name
    4. test with initially small window for prompt
    5. test with invalid keystrokes
    6. test with keystroke that leads into boundaries and rocks
    7. test with multiple players on babylon and plank
    8. test with bots
    9. test with narrower windows and narrower maps

4. Test `server` with valid arguments and the following:
    1. Testing with different maps without seed
    2. Testing with different maps with seed
    3. Testing with the same map, same seed and same player movements
    4. Testing with all differents maps, random number of bots

5. `grid` will mainly be tested in unit testing, and integration testing for `grid` will be mainly based on observation of its behaviors as it supports the server.

