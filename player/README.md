# CS50 nuggets-hemlock player

## Contents
* This subdirectory includes a `Makefile`, `player.c`, and `README.md`.

## Makefile

* In order to compile, simple 'make'
* Note that the `Makefile` relies on the support library be in the parent directory of player directory, as well as the `support.a` archive being made already in that directory

## Purpose
* The player.c file servers as the client for the nuggets game. User can join as player by providing the player name with the following syntax:
`./player hostname port`
Otherwise, user can join as spectator by using the following syntax:
`./player hostname port`
For details of the client protocals, refer to `REQUIREMENTS.md`(REQUIREMENTS.md). We recommend to redirect the stderr when running the program for better result.

## Implementation notes

* The player we implemented is compatible with our server. It will have proper display and promts if used with our server. During testing, we noticed that displays message can be printed more often and thus have a less clean interface with the Professor's server. This is because the professor's server implementation sends certain messages, such as `GOLD n p r` more frequently than our implementation. However, if this client is used with the server we provide, it will be fully functional and meets all aspects of (REQUIREMENTS.md). Therefore, in testing, we recommend using the server we provide for best results.
