#include <stdlib.h>
#include <stdio.h>
#include <curses.h>
#include <locale.h>
#include "main.h"
#include "curses_util.h"
#include "game.h"

int main() {
	setup_curses();

	play_new_game();

	end_curses();
	return 0;
}

