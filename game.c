#include <stdlib.h>
#include <ncurses.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "splash.h"
#include "game.h"
#include "ai.h"
#include "util.h"
#include "constants.h"

FILE *data;

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

void reverse_ball_direction() {
	ball.angle += M_PI;
	ball.angle = amod(ball.angle);
}

/* requires angle to be at most pi/2 away from ball.angle */
void reflect_ball(double angle) {
	ball.angle = amod(ball.angle);
	angle = amod(angle);
	double diff = angle - ball.angle;
	//assert(amod(diff) < M_PI / 2);
	ball.angle += 2 * diff;
	ball.angle = amod(ball.angle);
	reverse_ball_direction();
	update_ball_velocity();
}

void bound_ball_angle(double low, double hi) {
	ball.angle = amod(ball.angle);
	/* if hi is closer to ball.angle than low, we want to use hi, even
	 * if we must wrap around. Adding 2pi solves this. */
	ball.angle = fbound(ball.angle + 2 * M_PI, low + 2 * M_PI, hi + 2 * M_PI);
	ball.angle = amod(ball.angle);
	update_ball_velocity();
}

void update_ball_position() {
	ball.x += ball.velocity.x;
	ball.y += ball.velocity.y;

	if (ball.y < 0) {
		reflect_ball(M_PI);
		ball.y = 0;
	}
	if (BOARD_W < ball.y) {
		reflect_ball(0);
		ball.y = BOARD_W;
	}


	/*
	 *              -* ----|
	 *      h     /  |     |
	 *         /     |     |
	 *      /        |     |
	 *   /      l    |
	 * * ------------* -|  h
	 *   \ --__      |  |
	 *      \   \__ t|  d  |
	 *         \    -* -|  |
	 *      h     \  |     |
	 *              -* ----|
	 *
	 *  t = theta angle to reflect ball against
	 *  l = length from center of arc to paddle
	 *  h = height of paddle normalized
	 *  d = differece between ball.y and center of paddle
	 */
	if (ball.x < 0) {
		pthread_mutex_lock(&paddle_mut);
		double pad = paddle.p1.pos;
		pthread_mutex_unlock(&paddle_mut);
		double h = ((double)PADDLE_SIZE / (double)row);
		double l = (h / 2) / tan(M_PI / 6);
		double d = ball.y - pad;
		if (fabs(d) < h / 2) {
			double angle = amod((d > 0 ? M_PI : 0) + atan(l/d));
			double abs = angle_dist(angle, ball.angle);
			log_init();
			fprintf(log_file, "%f %f %f %f\n", d, angle, ball.angle, abs);
			if (abs< M_PI / 2) {
				reflect_ball(angle);
				bound_ball_angle(MIN_BALL_ANGLE, M_PI - MIN_BALL_ANGLE);
			}
			else {
				reflect_ball(3 * M_PI / 2);
			}
		}
		else {
			score.p2++;
			reflect_ball(3 * M_PI / 2);
		}
		ball.x = 0;
	}
	if (BOARD_W < ball.x) {
		pthread_mutex_lock(&paddle_mut);
		double pad = paddle.p2.pos;
		pthread_mutex_unlock(&paddle_mut);
		double h = ((double)PADDLE_SIZE / (double)row);
		double l = (h / 2) / tan(M_PI / 6);
		double d = pad - ball.y;
		if (fabs(d) < h / 2) {
			double angle = amod((d < 0 ? M_PI : 0) + atan(l/d));
			double abs = angle_dist(angle, ball.angle);
			log_init();
			fprintf(log_file, "%f %f %f %f\n", d, angle, ball.angle, abs);
			if (abs < (M_PI / 2)) {
				reflect_ball(angle);
				bound_ball_angle(M_PI + MIN_BALL_ANGLE, 2 * M_PI - MIN_BALL_ANGLE);
			}
			else {
				log_init();
				double abs = angle_dist(angle, ball.angle);
				fprintf(log_file, "%f %f %f %f\n", d, angle, ball.angle, abs);
				reflect_ball(M_PI / 2);
			}
		}
		else {
			score.p1++;
			reflect_ball(M_PI / 2);
		}
		ball.x = BOARD_W;
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
	draw_board(ball, score, paddle, col, row);
	splash();

	pthread_t getkeys;
	pthread_create(&getkeys, NULL, listen_keys, NULL);

	while (true) {
		usleep(GAME_SPEED);
		update_state();
		draw_board(ball, score, paddle, col, row);
	}

}
