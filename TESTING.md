# CS50 Nuggets testing


## Server

The `server.c` is the core program of this project, and is tested by the following steps, all testing using the given compiled player :

* Testing with differernt invalid command line arguments to see if the program is able to catch those errors. Include all the possible situation: invalid number of arguments, invalid path for the maps.
* Testing with different maps and one player without seed to check if the game start correctly.
* Testing with different maps, one player and a spectator without seed, to check if the player is able to move using the keystroke in the spec.
* Testing the same map, one player with seed to check the gold collection:  1. player is able to collect gold at the pile  2. the gold pile is back to the room spot after the player collect the gold 3. player can collect all the gold, and game exit normally after all gold is collected
* Testing with all differents maps, random number of bots, and spectat any bad game logic, such as location switch



## Player

The `player` directory has `player.c`, which is mainly tested through integration testing for functionalities. 

Unit testing wise, the following are done:

* Testing the client with wrong number of arguments, and the `player.c` file will print a usage line if wrong number of arguments is provided.
* Testing the client with correct number of arguments, but invalid port string or host. In this case，the client will print error message about failed server address initialization and exits 1.
* Testing the client with screen smaller than the grid size requirement. In this case, the client will prompt user to enlarge screen and press ENTER after screen is large enough. Note that pressing ENTER while screen is not large enough will not result in the game proceeding, and neither does pressing keys other than ENTER.
* Testing with valgrind to ensure no memory leaks. The only leak allowed was the "still reachable" leaks from the ncurses library, which is ignored per spec.


## Grid

The `grid` directory has `gridtest.c`, which can be compiled by the `make test` target.

`gridtest.c` is a simple unit test. It is limited in many ways as full interaction with the `grid` is impossible without the `server` and the `client`. A more extensive test will be performed in integration testing.

With `maps/main.txt`, we test the following functionalities:

* test `grid_load` with invalid file name.
* test `grid_load` with `maps/main.txt`.
* test `grid_toString` by printing the result.
* test `grid_nrow` and `grid_ncol`
* test `grid_update` and `grid_getchar` with out of bounds row and col values.
* test `grid_update` and `grid_getchar` by checking if the character has been properly updated at the given spot.
* test wrapper functions for `grid_getchar`. (`grid_isRock`, `grid_canMoveTo`, `grid_isRock`, `grid_isPlayer`, `grid_isEmptyRoomSpot`)
* test `grid_setVisibility`. Print out the visible portion of the grid for the given location. Tested with various points including passages. 



## Integration testing

The integration testing has been done by combining all the modules we have and testing with different keystrokes, maps, seeds, and player behaviors.

1. We begin our integration testing by using the `main.txt` map which is standard size, and relatively low challenge for visibility. We then initiate only one player to collect gold. In this circumstance, we test the player's keystroke and movements, as well as the validity of visibility. We allow the player to use all 16 allowed movement keystrokes and observed the movement pattern. In addition, we check whether player is able to follow the visibility protocals and has view blocked by the wall and passageway. Furthermore, we observe whether player movement is hindered by wall and rock.
2. We further test our game system by allowing multiple players. In this case, on the basis of basic movement patterns, we also observe the behavior when players overstep on each other. We make sure that they correctly switch places, not only on their own displays, but on the spectator's display as well. In addition, we test with unknown keystrokes, and observe that the server sends back an error message, and player simply logs the error and ignores it. However, we do observe the situation that `down arrow` can be recognized as `[B` in Unix code, and ncurses thus parses the down arrow and sends the message to server, which server will parse the bracket and capital B separately, thus resulting in the same effect as a single capital B will have on the movement. We consider this to be an odd case and would not affect overall gameplay, and may be a limitation to Unix code.
3. After basic movement pattern has been proven correct, we observe the gold messages to ensure that it correctly displays on each player's screen respectively, as well as a special spectator's message. We allow players to move, collect gold, and quit halfway through the game, to ensure that "players are not allowed to rejoin or be replaced" per spec, and we tested that spectator can be replaced, which kicks out the previous spectator, per spec. In addition, we observed the end game summary table and ensured that it prints out correctly for each player, and includes info about the player who quited as well.
4. Furthermore, we vary our testing on different maps, and with different seeds. We ensured that seeds worked, as player are dropped to the same locations every time, the gold pile randomized to the same number if it's collected in the same order. We tried maps with harder visibility such as `challenge.txt` and observed game behavior.
5. Lastly, we ran with valgrind on the player side, and the server side to ensure no memory leak besides those caused by ncurses, which turned out to be the case.