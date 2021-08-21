#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ncurses.h>
#include <ctype.h>
#include "message.h"
#include "log.h"

typedef struct gameInfo{

    int GridRows; //height of grid
    int GridCols; //width of grid
    addr_t serverAddr; //address of server
    bool isPlayer; //true if is player, false if is spectator
    char letter; //letter that represents the player
    char* portStr; //string pointer for the portstr
} gameInfo_t;

static gameInfo_t game; //global struct that holds information


/******** function prototypes *************/
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleInput(void* arg);
static void game_init(void);
static void parseArgs(const int argc, const char* argv[]);
static void parse_GRID(char* msg);
static void parse_GOLD(char* msg);
/*************************************/

/*
 * simply parse args, call message loop, and call message_done
 */
int main(const int argc, const char* argv[])
{
    //use this if want to log to file instead
    /*
    FILE* fp = fopen("playerlog", "w");
    if(fp == NULL){
        fprintf(stderr, "fp open failed\n");
        exit(1);
    }
    */
    log_init(stderr);
    message_init(stderr);

    parseArgs(argc, argv);
    message_loop(NULL, 0, NULL, &handleInput, &handleMessage);
    message_done();
    log_done();
    

}

/*
 * initialize the screen and sets up the ncurse library
 * prompts the user to enlarge window if necessary
 */
static void
game_init(void)
{
    initscr(); // initialize the screen
    cbreak();  // accept keystrokes immediately
    noecho();  // don't echo characters to the screen
    start_color(); // turn on color controls
    init_pair(1, COLOR_WHITE, COLOR_BLACK); // define color 1
    attron(COLOR_PAIR(1)); // set the screen color using color 1
    curs_set(0);
    int ly, lx; // upper left
    int uy, ux; // lower right
    // get the boundaries of the screen
    getbegyx(stdscr, ly, lx);
    getmaxyx(stdscr, uy, ux);

    //check if window is big enough
    int windowWidth = ux - lx;
    int windowHeight = uy - ly;

    //Prompt player to enlarge window if it's smaller than grid size
    if (windowWidth < (game.GridCols+1) || windowHeight < (game.GridRows+1)){
        printw("Enlarge your window to \n%d high and %d wide.\nPress ENTER to continue", game.GridRows+1, game.GridCols+1);
        int ch;
        ch = getch();
        //wait for user to press enter after window has been enlarged
        while (windowWidth < (game.GridCols + 1) || windowHeight < (game.GridRows + 1) || ch != 10){
            getbegyx(stdscr, ly, lx);
            getmaxyx(stdscr, uy, ux);
            windowWidth = ux - lx;
            windowHeight = uy - ly;
            ch = getch();
        }

    }

    move(0,0);
    refresh();


}



/*
 * This function handles parsing of commandline arguments.
 * The server address is set here and stored in gameInfo struct
 * Depending the client as spectator or player, message is sent to server accordingly
 */
static void
parseArgs(const int argc, const char* argv[])
{
    char* realName;
    if (argc == 3){
        game.isPlayer = false;
        
    } else if (argc == 4){
        realName = (char*)argv[3];
        game.isPlayer = true;
    } else {
        fprintf(stderr, "usage:./player hostname port [playername] \n");
        exit(1);
    }
    char* hostname = (char*)argv[1];
    char* port = (char*) argv[2];

    //sets the port according to commanline
    game.portStr = port;
    addr_t tempAddr = message_noAddr();

    //check if the port and hostname are correct
    if (message_setAddr(hostname, port, &tempAddr) == false){
        fprintf(stderr, "IP address validation based on provided hostname and portstring failed\n");
        log_e("IP address validation failed\n");
        exit(1);
    } 
    game.serverAddr = tempAddr;
    
    if (game.isPlayer){
        //sends the player message with name
        char* temp = "PLAY ";
        char* fullName = malloc(sizeof(char)*1000);
        if (fullName == NULL){
            log_e("Out of memory for name\n");
            exit(1);
        }   
        sprintf(fullName, "%s%s", temp, realName);
        message_send(tempAddr, fullName);
        free(fullName);
    } else {
        //sends the spectator message
        message_send(game.serverAddr, "SPECTATE");
    }    
}

/*
 * this function deals with keystroke parsing and sends key to server accordingly.
 */
static bool
handleInput(void* arg)
{

    int c = getch();
    if(game.isPlayer){
        switch(c) {
        case ('h'):   message_send(game.serverAddr, "KEY h"); refresh(); return false; //move left
        case ('l'):   message_send(game.serverAddr, "KEY l"); refresh(); return false; // move right
        case ('j'):   message_send(game.serverAddr, "KEY j"); refresh(); return false; // move down
        case ('k'):   message_send(game.serverAddr, "KEY k"); refresh(); return false; // move up
        case ('y'):   message_send(game.serverAddr, "KEY y"); refresh(); return false; //move diagonally up and left
        case ('u'):   message_send(game.serverAddr, "KEY u"); refresh(); return false; // move diagonally up and right
        case ('b'):   message_send(game.serverAddr, "KEY b"); refresh(); return false; //move diagonally down and left
        case ('n'):   message_send(game.serverAddr, "KEY n"); refresh(); return false; //move diagonally down and right
        case ('H'):   message_send(game.serverAddr, "KEY H"); refresh(); return false; //move left
        case ('L'):   message_send(game.serverAddr, "KEY L"); refresh(); return false; // move right
        case ('J'):   message_send(game.serverAddr, "KEY J"); refresh(); return false; // move down
        case ('K'):   message_send(game.serverAddr, "KEY K"); refresh(); return false; // move up
        case ('Y'):   message_send(game.serverAddr, "KEY Y"); refresh(); return false; //move diagonally up and left
        case ('U'):   message_send(game.serverAddr, "KEY U"); refresh(); return false; // move diagonally up and right
        case ('B'):   message_send(game.serverAddr, "KEY B"); refresh(); return false; //move diagonally down and left
        case ('N'):   message_send(game.serverAddr, "KEY N"); refresh(); return false; //move diagonally down and right
        case ('Q'):   message_send(game.serverAddr, "KEY Q"); refresh(); return false; //move diagonally down and right
        default: 
                      mvprintw(0, 50,"unknown keystroke", c);
                      char temp[100];
                      sprintf(temp, "KEY %c",c);
                      message_send(game.serverAddr, temp); refresh(); return false; //not allowed   

        }
    } else {
        switch(c){

            case ('Q'):   message_send(game.serverAddr, "KEY Q"); refresh(); return false; //move diagonally down and right
            default: return false;
        }
    }
}

/*
 * parses the incoming messages and calls functions to manage the message accordingly
 */

static bool 
handleMessage(void* arg, const addr_t from, const char* message)
{
    if (message == NULL){return false;}

        if (strncmp(message, "GRID ", strlen("GRID ")) == 0) {
            char *content = (char*)message + strlen("GRID ");
            parse_GRID(content);
            return false;
        }
        if (strncmp(message, "GOLD ", strlen("GOLD ")) == 0) {
            char *content = (char*)message + strlen("GOLD ");
            parse_GOLD(content);
            return false;
        }
        if (strncmp(message, "DISPLAY\n", strlen("DISPLAY\n")) == 0) {
            char *content = (char*)message + strlen("DISPLAY\n");
            mvprintw(1,0, "%s", content);
            refresh();
            return false;
        }
        if (strncmp(message, "QUIT ", strlen("QUIT ")) == 0) {
             char *content = (char*)message + strlen("QUIT ");
             endwin(); // turn off curses display
             printf("%s\n", content); //prints endgame table
             return true;
        }
        if (strncmp(message, "ERROR ", strlen("ERROR ")) == 0) {
             char *content = (char*)message + strlen("ERROR ");
             refresh();
             log_e(content);
             return false;
        }
        if (strncmp(message, "OK ", strlen("OK ")) == 0) {
             char *content = (char*)message + strlen("OK ");
             char letter;
             sscanf(content, "%c", &letter);
             game.letter = letter;
             return false;
        }
        return false;

}

/* 
 * breaks down the grid message to set up gridcols and gridrows
 */
static void
parse_GRID(char* msg)
{
    char* str;
    str = strtok(msg, " ");
 
    int rows = atoi(str);
    game.GridRows = rows;
    str = strtok(NULL, " ");

    int cols = atoi(str);
    game.GridCols = cols;

    //initialize game
    game_init();
    refresh();
}

/*
 * breaks down the gold message and displays on screen
 */
static void
parse_GOLD(char* msg)
{
    char* str;
    str = strtok(msg, " ");
    int justCollected = atoi(str);
    str = strtok(NULL, " ");
    int totalCollected = atoi(str);
    str = strtok(NULL, " ");
    int goldLeft = atoi(str);
    move(0,0);

    //clears line 1
    refresh();
    clrtoeol();
    refresh();

    if(game.isPlayer){

        //displays the message based on how much gold is connected
        if (justCollected == 0){
            mvprintw(0,0, "Player %c has %d nuggets (%d nuggets unclaimed).", game.letter,totalCollected, goldLeft);
        } else {
            mvprintw(0,0,"Player %c has %d nuggets (%d nuggets unclaimed).  GOLD received: %d", game.letter,totalCollected, goldLeft, justCollected);
        }
    } else {
        //spectator displays gold message
        mvprintw(0,0, "Spectator: %d nuggets unclaimed. Play at plank %s", goldLeft, game.portStr);
    }
    refresh();
}


