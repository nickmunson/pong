#include <stdio.h>
#include <fann.h>
#include "util.h"

struct fann *ann = NULL;

int make_move(double myPos, double theirPos, double ballX, double ballY) {
    fann_type *calc_out;
    fann_type input[4];

    if (!ann) {
		ann = fann_create_from_file("pong_float.net");
	}

	log_init();

    input[0] = myPos;
    input[1] = theirPos;
	input[2] = ballX;
	input[3] = ballY;
    calc_out = fann_run(ann, input);

	//fprintf(log_file, "%f\n", *calc_out);
	if (*calc_out > 0) {
		return 1;
	}
	else {
		return -1;
	}
}
