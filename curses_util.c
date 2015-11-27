#include <curses.h>
#include <locale.h>

void setup_curses()
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    nonl();
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);
	curs_set(0);
    return;
}

void end_curses() {
	endwin();
}
