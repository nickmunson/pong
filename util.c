#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "util.h"

FILE *log_file = NULL;

double amod(double angle) {
	double ret = fmod(angle, 2 * M_PI);
    return (ret < 0 ? ret + 2 * M_PI : ret);
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

double fbound(double val, double lower, double upper) {
    if (val < lower) {
        return lower;
    } else if (upper < val) {
        return upper;
    } else {
        return val;
    }
}

double angle_dist(double a1, double a2) {
	a1 = amod(a1);
	a2 = amod(a2);

	if (a1 > a2) {
		return a1 - a2;
	}
	else {
		return a2 - a1;
	}
}

void assert(bool cond) {
    if (!cond) {
        exit(-1);
    }
}


void log_init() {
    if (!log_file) {
		log_file = fopen("log", "a");
	}
}
