#ifndef UTIL_H
#define UTIL_H

#include <stdbool.h>

#define ftoi(x) (x >= 0 ? (int)(x+0.5) : (int)(x-0.5))

double amod(double angle);

int bound(int val, int lower, int upper);

double fbound(double val, double lower, double upper);

double angle_dist(double a1, double a2);

void assert(bool cond);

FILE *log_file;

void log_init();

#endif
