#include <curses.h>
#include <string.h>
#include "splash.h"
#include "game.h"

char *welcome = "Welcome to Pong";
char *presskey = "<< Press Any Key >>";

void splash() {
	int row, col;
	getmaxyx(stdscr,row,col);
	mvprintw(row/2-1,col/2-strlen(welcome)/2,welcome);
	mvprintw(row/2,col/2-strlen(presskey)/2,presskey);
	getch();
	clear();
}
