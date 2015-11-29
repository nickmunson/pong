#ifndef DRAW_H
#define DRAW_H

#include "game.h"

void draw_board(struct ball_s ball, struct score_s score,
		struct paddle_s paddle,int rows, int cols);

#endif
