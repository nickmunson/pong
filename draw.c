#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "util.h"
#include "draw.h"
#include "game.h"
#include "constants.h"

struct dimension {
	int rows;
	int cols;
};

void draw_stats(struct score_s score, struct dimension dim) {
	char stats[100];
	snprintf(stats, 100, "(%i)            (%i)", score.p1, score.p2);
	mvprintw(0,dim.cols/2-strlen(stats)/2,stats);
}

void draw_bar(double pos, int col, struct dimension dim) {
	int y = ftoi((pos / BOARD_H) * dim.rows);
	y = bound(y, PADDLE_SIZE/2, dim.rows - PADDLE_SIZE/2);

	for(int i=-PADDLE_SIZE/2; i<PADDLE_SIZE/2; i++) {
		mvprintw(y+i,col,"|");
	}
}

void draw_paddles(struct paddle_s paddle, struct dimension dim) {
	draw_bar(paddle.p1.pos, 0, dim);
	draw_bar(paddle.p2.pos,dim.cols-1, dim);
}

void draw_ball(struct ball_s ball, struct dimension dim) {
	mvprintw(ball.y * dim.rows, ball.x * dim.cols,"@");
}

void draw_board(struct ball_s ball, struct score_s score,
		struct paddle_s paddle, int cols, int rows) {
	struct dimension dim;
	dim.rows = rows;
	dim.cols = cols;

	clear();

	draw_stats(score, dim);
	draw_paddles(paddle, dim);
	draw_ball(ball, dim);

	refresh();
}

