#ifndef CONSTANTS_H
#define CONSTANTS_H

#define PADDLE_SIZE 8
#define PADDLE_SPEED 0.001
#define GAME_SPEED 1000 // one timer tick per x microseconds

#define BALL_SPEED 0.0010
#define BALL_START_ANGLE (M_PI/2 + M_PI/8) //(M_PI/2) // 0 is down, pi/2 is right
#define MIN_BALL_ANGLE (M_PI / 5)

#define BOARD_W ((double)(1.0))
#define BOARD_H ((double)(1.0))

#define PADDLE_UP 1
#define PADDLE_STATIONARY 0
#define PADDLE_DOWN -1

#endif
