#ifndef MATHFUNCTIONS_H
#define MATHFUNCTIONS_H

#include <math.h>

//DO NOT USE DEFINES, USE THE BLOODY FUNCTIONS
#define TO_RAD_SUPER_INTERNAL_DO_NOT_USE 0.01745329f
namespace KMath{


inline static double TO_RAD(double X) { return X*TO_RAD_SUPER_INTERNAL_DO_NOT_USE;}
inline static double TO_DEG(double X) { return X/TO_RAD_SUPER_INTERNAL_DO_NOT_USE;}

#define DISTANCE(x1,x2,y1,y2) (sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2)))

template<typename T> inline T norm2(T dx, T dy)
{
	return sqrt((dx) * (dx) + (dy) * (dy));
}
inline static double wrapToPi(double angle)
{
	while (angle > M_PI)
		angle -= 2.0 * M_PI;

	while (angle < -M_PI)
		angle += 2.0 * M_PI;

	return angle;
}

inline static double wrapTo2Pi(double angle)
{
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;

	while (angle < -2.0 * M_PI)
		angle += 2.0 * M_PI;

	return angle;
}

inline static double wrapTo0_2Pi(double angle)
{
	while (angle > 2.0 * M_PI)
		angle -= 2.0 * M_PI;

	while (angle < 0)
		angle += 2.0 * M_PI;

	return angle;
}

inline static double anglediff2(double a1, double a2)
{
	return wrapToPi(wrapToPi(a1 + M_PI - a2) - M_PI);
}

inline static double anglediff(double a1, double a2)
{
	return fabs(wrapTo0_2Pi(a1 + M_PI - a2) - M_PI);
}

static double toPolarD(double x, double y)
{
	return sqrt(x * x + y * y);
}

static double toPolarT(double x, double y)
{
	return atan2(y, x);
}

static double toCartesianX(double d, double t)
{
	return d * cos(t);
}

static double toCartesianY(double d, double t)
{
	return d * sin(t);
}

}
#endif
