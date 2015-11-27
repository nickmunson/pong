#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "splash.h"
#include "game.h"
#include "ai.h"

#define PADDLE_SIZE 6
#define PADDLE_SPEED 0.001
#define GAME_SPEED 1000 // one timer tick per x microseconds

#define BALL_SPEED 0.0007
#define BALL_START_ANGLE (M_PI/2 + M_PI/8) //(M_PI/2) // 0 is down, pi/2 is right

#define BOARD_W ((double)(1.0))
#define BOARD_H ((double)(1.0))

#define PADDLE_UP 1
#define PADDLE_STATIONARY 0
#define PADDLE_DOWN -1

#define ftoi(x) (x >= 0 ? (int)(x+0.5) : (int)(x-0.5))

FILE *data;

struct score_s {
	int p1;
	int p2;
};

struct paddle_player_s {
	double pos;
	int dir;
};

struct paddle_s {
	struct paddle_player_s p1;
	struct paddle_player_s p2;
};

struct velocity_s {
	double x;
	double y;
};

struct ball_s {
	double x;
	double y;
	double angle;
	struct velocity_s velocity;
};

struct paddle_s paddle;
pthread_mutex_t paddle_mut = PTHREAD_MUTEX_INITIALIZER;
struct ball_s ball;
struct score_s score;

static int row, col;

void update_ball_velocity() {
	ball.velocity.x = BALL_SPEED * sin(ball.angle);
	ball.velocity.y = BALL_SPEED * cos(ball.angle);
}

void setup() {
	getmaxyx(stdscr,row,col);
	paddle.p1.pos = BOARD_H / 2;
	paddle.p2.pos = BOARD_H / 2;
	paddle.p1.dir = PADDLE_STATIONARY;
	paddle.p2.dir = PADDLE_STATIONARY;
	ball.x = 0.5;
	ball.y = 0.5;
	ball.angle = BALL_START_ANGLE;
	update_ball_velocity();
	score.p1 = 0;
	score.p2 = 0;
}

void draw_stats() {
	char stats[100];
	snprintf(stats, 100, "(%i)            (%i)", score.p1, score.p2);
	mvprintw(0,col/2-strlen(stats)/2,stats);
}

int bound(int val, int lower, int upper) {
	if (val < lower) {
		return lower;
	} else if (upper < val) {
		return upper;
	} else {
		return val;
	}
}

void draw_bar(double pos, int col) {
	int y = ftoi((pos / BOARD_H) * row);
	y = bound(y, PADDLE_SIZE/2, row - PADDLE_SIZE/2);

	for(int i=-PADDLE_SIZE/2; i<PADDLE_SIZE/2; i++) {
		mvprintw(y+i,col,"|");
	}
}

void draw_paddles() {
	pthread_mutex_lock(&paddle_mut);
	draw_bar(paddle.p1.pos, 0);
	draw_bar(paddle.p2.pos,col-1);
	pthread_mutex_unlock(&paddle_mut);
}

void draw_ball() {
	mvprintw(ball.y * row, ball.x * col,"@");
}

void draw_board() {
	clear();
	draw_stats();
	draw_paddles();
	draw_ball();
	refresh();
}

double fbound(double val, double lower, double upper) {
	if (val < lower) {
		return lower;
	} else if (upper < val) {
		return upper;
	} else {
		return val;
	}
}

void reverse_ball_direction() {
	ball.angle += M_PI;
	ball.angle = fmod(ball.angle, 2 * M_PI);
}

void reflect_ball(double angle) {
	double diff = angle - ball.angle;
	ball.angle += 2 * diff;
	reverse_ball_direction();
	update_ball_velocity();
}

void update_ball_position() {
	ball.x += ball.velocity.x;
	ball.y += ball.velocity.y;

	if (ball.y <= 0) {
		reflect_ball(M_PI);
	}
	if (1 <= ball.y) {
		reflect_ball(0);
	}


	if (ball.x <= 0) {
		pthread_mutex_lock(&paddle_mut);
		double pad = paddle.p1.pos;
		pthread_mutex_unlock(&paddle_mut);
		double paddle_h = ((double)PADDLE_SIZE / 2 / (double)row);
		double diff = pad - ball.y;
		double dist = fabs(diff);
		if (paddle_h <= dist) {
			score.p2++;
			reflect_ball(3 * M_PI / 2);
		}
		else {
			reflect_ball(cos(diff/paddle_h));
		}
	}
	if (1 <= ball.x) {
		pthread_mutex_lock(&paddle_mut);
		double pad = paddle.p2.pos;
		pthread_mutex_unlock(&paddle_mut);
		double paddle_h = ((double)PADDLE_SIZE / 2 / (double)row);
		double diff = pad - ball.y;
		double dist = fabs(diff);
		if (paddle_h < dist) {
			score.p1++;
			reflect_ball(M_PI / 2);
		}
		else {
			reflect_ball(sin(diff/paddle_h));
		}

	}
}

void update_paddles() {
	/* lock paddle */
	pthread_mutex_lock(&paddle_mut);

	switch (paddle.p1.dir) {
		case PADDLE_UP:
			paddle.p1.pos -= PADDLE_SPEED;
			break;
		case PADDLE_DOWN:
			paddle.p1.pos += PADDLE_SPEED;
			break;
		default:
			break;
	}
	switch (paddle.p2.dir) {
		case PADDLE_UP:
			paddle.p2.pos -= PADDLE_SPEED;
			break;
		case PADDLE_DOWN:
			paddle.p2.pos += PADDLE_SPEED;
			break;
		default:
			break;
	}

	double frow = (double)(row);
	double min = (((double)PADDLE_SIZE) / 2) / frow;
	double max = (frow - (((double)PADDLE_SIZE) / 2)) / frow;
	paddle.p1.pos = fbound(paddle.p1.pos,min,max);
	paddle.p2.pos = fbound(paddle.p2.pos,min,max);

	/* unlock paddle */
	pthread_mutex_unlock(&paddle_mut);
}

void record_player(FILE *fp, int player) {
	double myPos, theirPos, ballX;
	int dir;

	if (player == 1) {
		dir = paddle.p1.dir;
		myPos = paddle.p1.pos;
		theirPos = paddle.p2.pos;
		ballX = ball.x;
	}

	if (player == 2) {
		dir = paddle.p2.dir;
		myPos = paddle.p2.pos;
		theirPos = paddle.p1.pos;
		ballX = fabs(ball.x - BOARD_W);
	}

	fprintf(fp, "%f %f %f %f\n%d\n", myPos, theirPos, ballX, ball.y, dir);
}

void update_state() {
	//paddle.p1.dir = make_move(paddle.p1.pos, paddle.p2.pos, ball.x , ball.y);
	update_ball_position();
	update_paddles();
}

void* listen_keys() {
	int ch;
	while((ch = getch()) != ERR) {
		switch(ch) {
			case 'a':
				pthread_mutex_lock(&paddle_mut);
				paddle.p1.dir = PADDLE_UP;
				pthread_mutex_unlock(&paddle_mut);
				record_player(data, 1);
				break;
			case ';':
				pthread_mutex_lock(&paddle_mut);
				paddle.p1.dir = PADDLE_DOWN;
				pthread_mutex_unlock(&paddle_mut);
				record_player(data, 1);
				break;
			case KEY_UP:
				pthread_mutex_lock(&paddle_mut);
				paddle.p2.dir = PADDLE_UP;
				pthread_mutex_unlock(&paddle_mut);
				break;
			case KEY_DOWN:
				pthread_mutex_lock(&paddle_mut);
				paddle.p2.dir = PADDLE_DOWN;
				pthread_mutex_unlock(&paddle_mut);
				break;
		}
	}

	return NULL; // this should be waited on somewhere, kek
}


void play_new_game() {
	data = fopen("pong.data", "a");
	setup();
	draw_board();
	splash();
	//draw_board();

	pthread_t getkeys;
	pthread_create(&getkeys, NULL, listen_keys, NULL);

	while (true) {
		usleep(GAME_SPEED);
		update_state();
		draw_board();
	}

}
