#ifndef GAME_H
#define GAME_H

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

void play_new_game();
void draw_board();

#endif
