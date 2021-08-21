# CS50-nuggets-hemlock
## Design spec
Nuggets is a game that can be played via webserver, and users can join as either as players or spectators. The core program is divided into 


- User interface
- Inputs and outputs
- Functional decomposition into modules
- Pseudo code for logic/algorithmic flow
- Major data structures
- Testing plan


## User interface

### Client
As in requirements, to start the game:
```bash
./player hostname port [playername]
```
`hostname` and `port` must be included, and `playername` is optional.
Player should use keys to control the move; refer to the requirement spec for details. Player will receive displayed string of what he/she is allowed to see.


### Server
As in Requirements Spec, to start the server, it must include one argument `map.txt`, and optional argument `seed`.
```
./server map.txt [seed]
```
The `seed` determines the random behavior of the map, such as the location of gold and players. Server contains information on current gold status, player location, and a master grid.


## Inputs and outputs

### Client
The client takes as input the hostname/IP address and port number that the server is listening to. If the user joins as a spectator if a playername is not specified. Client outputs with keystrokes and quiting.

### Server
The server take as input a text file that generates the map of the game. The seed is an optional parameter used to determines the random behavior of the map, such as the location of gold and players. Server outputs wil display strings, and game status.

## Functional decomposition into modules
We anticipate the following modules:

 1. *player*, which handles message sending and receiving from client side and has different behaviors for players and spectators.
 2. *server*, which handles message sending and receiving on server side while keeping data such as player status, gold status, and game status.
 3. *grid*, which validates and loads map into an internal grid data structure. Provides function for telling what to display for specific location and handles displaying.

And the following funtions for client:
 1. *main*, for the player to parse the argument and initialize the client module as a player
 2. *playerTimeout*, handles timeout for player
 3. *playerInput*, handles command line inputs and keystrokes
 4. *userMessage*, handles message sending and receiving

And the following funtions for server:
 1. *main* for the server to parse the argument, initilize the server and game struct
 2. *server_handleTimeout*, handles timeout for server
 3. *server_handleInput*, handles input from commandline
 4. *server_handleMessage*, handles message sending and receiving
 5. *server_handle_removal*, handles the case when player is removed
 6. *server_visibility_display*, handles what player can see from current location
 7. *server_name_truncate*, truncates player name to allowed length
 8. *server_drop_gold*, drops random piles of gold with random counts


And the following funtions for grid:
 
 1. *grid_row*, takes a grid and returns the number of rows.
 2. *grid_col*, takes a grid and returns the number of columns.
 3. *grid_load*, creates a 2d array from the given map text file. Infers number of rows and columns?
 4. *grid_visible*, returns a 2d array of visible points at the given location for the given grid.
 5. *grid_update*, updates the character at the given point.
 6. *grid_display*, creates a string message of the given grid.
 7. *grid_isWall*, simply checks whether the given location is wall and returns a boolean.
8. *grid_generateGold* ,drops random piles of gold in random locations within specified range 

## Pseudo code for logic/algorithmic flow

### Player
The player will run as follows:

        Parses the command line, validates parameters
        Determines and stores user type as player or spectator:
            if player:
                initialize message loop with playerTimeout, playerInput, userMessage;
            if spectator:
                initialize message loop with spectatorTimeout, spectatorInput, and userMessage.

where playerTimeout:

        No necessity for handling timeout, simple pass NULL

where playerInput:
        
        if player:
            runs with the ncurses library
            continues to read input from player to know what keystroke to send
            handles inputs for 'QUIT' by message server with 'QUIT' type message
        if spectator:
            doesn't do anything


        
where userMessage:

        Parses message based on whether user is player or specator.
        As player:
            sends keystrokes to server
            receives Gold information and 'Display' string
            handles 'DISPLAY' string 
            updates local game struct 
            Repaints based on local game struct's decision on visibility
        As spectator:
            Sends 'SPECTATE' status
            receives map status
            receives players' locations
            Paints whole map from local struct and all player location
            
### Server

The server will run as follows:

        parses the command line, validate parameters;
        calls server_drop_gold to initialize gold;
        initialize game struct with information of player and gold;
        maintains master knowledge of player location and gold status, stored in game struct;
        initialize message loop with handleTimeout, handleInput, and handleMessage;

where server_handleTimeout:

        No necessity to handle timeout, simply pass NULL

where server_handleInput:
        
        No necessity for input reading for server, simply pass NULL
        
        
where server_handleMessage:

        Parses message by message type;
        if message type is 'PLAY':
            Checks whether max player has been reached:
                if so, reply with 'QUIT' message;
            Checks validity of name:
                if name is valid:
                    call name_truncate with name and allowed length;
                    Decide letter for player;
                    Reply with 'OK Letter' message;
                    Reply with 'GRID' and 'GOLD' message;
                    Stores processed name of player;
                if name is invalid:
                    Responds with 'QUIT' message;
        if message type is 'SPECTATE':
            Reply with 'GRID' and 'GOLD' message'
            continue to update this spectator with complete display in loop
        if message type is 'QUIT':
            calls server_handle_removal
        if message type is 'KEY':
            validate whether key is allowed
            if key is allowed:
                update player location in struct
                update gold info if changed
                update the master display(for server and spectator to see)
                for all the player:
                    Determine their visibility(call visibility_display)
                    Reply with 'DISPLAY' message accordingly
                for all spectators:
                    Reply with master 'DISPLAY' message
        

where server_handle_removal:

        Takes a player name
        removes it from game struct
        remove symbol of player on map
        
where server_visibility_display:

        Takes a grid, an location in the grid;
        call grid_visible;
        call grid_display using output of grid_vision;
        
        
            
where server_name_truncate:

        Takes a string and max length allowed
        Truncates the string based on length allowed
        Replace illegal characters with underscore
        Return truncated string


where server_drop_gold:
    
        drops at least GoldMinNumPiles and at most GoldMaxNumPiles of gold in the room

## Major data structures
Helper module provides the nessesary data structures:
* *grid*: 2D char array 
    * char array[x coodinate][y coordinate] -> stored char
* *player*: 
* *server*: master grid and serveral hashtable
    * game struct:
        * master grid: all information about map and gold
        * IPhashtable: player name as key, IP address as hashtable
        * locationHashtable: player name as key, his current location as item
        
        * seenHashtable: player name as key, 2D array of grid parts he has seen as item
        * goldCounter: player letter (casted as integer) as key, gold collected as count
        * int goldTotal: total gold that exists in the game
        * int goldLeft: remaining gold left in the game
        * int goldCollected: number of gold collected in the game
        * int goldPileCount[x][y] = number of gold at that location


## Team work distribution
Jeff and Leo will starting working on the server, and Tim and Tay on the grid.
First, our team will agree upon the general structure of the program and agree on data structures and variables used. Then, Jeff and Leo will have the server set up enough to be able to run message loops, while Tay and Tim prepare the grid to be able to parse maps and return display Strings. Then, we will test the server based on the strings concatenated from the grid. Once that works, we will know that the outputs of the grids can be used for the server, and thus proceed to work our own parts. 
Once these two components are finished, we will converge to work on the client together. 

## Testing plan
1. Simple test for the command line arguments parsing

We will test the client and server with several         command line inputs, including various number of inputs and invalid command line arguments. We will check is server and client initialize correctly based on the input.

2. Unit test for server and grid

We will implement the server and grid first, which will be tested with the example client that has been provided to us. We will note whether messages are being sent correctly between server and clients, and whether game information can be updated succesfully. In addition, we will use the babylon servers to test with multiple clients.

3. System testing
 
Once the client is implemented, we plan to use the babylon servers and nugget bots to test whether the game works correctly.
