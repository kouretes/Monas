#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <math.h>

static double wrapToPi(double angle) {
	while (angle > M_PI)
		angle -= 2.0 * M_PI;
	while (angle < -M_PI)
		angle += 2.0 * M_PI;
	return angle;
}

static double wrapTo2Pi(double angle) {
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < -2.0 * M_PI)
		angle += 2.0 * M_PI;
	return angle;
}

static double wrapTo0_2Pi(double angle) {
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;
	while (angle < 0)
		angle += 2.0 * M_PI;
	return angle;
}

static double anglediff2(double a1, double a2) {
	return wrapToPi(wrapToPi(a1 + M_PI - a2) - M_PI);
}

static double anglediff(double a1, double a2) {
	return fabs(wrapTo0_2Pi(a1 + M_PI - a2) - M_PI);
}	

static double toPolarD(double x, double y) {
	return sqrt(x*x + y*y);
}

static double toPolarT(double x, double y) {
	return atan2(y, x);
}

static double toCartesianX(double d, double t) {
	return d*cos(t);
}

static double toCartesianY(double d, double t) {
	return d*sin(t);
}


#endif
