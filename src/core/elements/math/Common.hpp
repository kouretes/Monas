#ifndef MATHCOMMON_H
#define MATHCOMMON_H

#include <math.h>
#include <limits>

//DO NOT USE DEFINES, USE THE BLOODY FUNCTIONS
#define TO_RAD_SUPER_INTERNAL_DO_NOT_USE 0.01745329f
namespace KMath{

const float INF = std::numeric_limits<float>::infinity();

inline static double TO_RAD(double X) { return X*TO_RAD_SUPER_INTERNAL_DO_NOT_USE;}
inline static double TO_DEG(double X) { return X/TO_RAD_SUPER_INTERNAL_DO_NOT_USE;}

template<typename T> inline T DISTANCE(T x1, T x2, T y1, T y2) {
	return sqrt((x1-x2)*(x1-x2) + (y1-y2)*(y1-y2));
}

template<typename T> inline T norm2(T dx, T dy) {
	return sqrt((dx) * (dx) + (dy) * (dy));
}

template<typename T> inline T Min(T a, T b) {
	return (a < b) ? a : b;
}

template<typename T> inline T Max(T a, T b) {
	return (a > b) ? a : b;
}

/**
 * @fn template<typename T> inline T FUNC1ByX(T x)
 * @brief function f(x)=1/x used to generate supporters positions.
 * @return the result of 1/x or infinity number in case x = 0.
 */
template<typename T> inline T func1ByAbsX(T x) {
	if (x == 0)
		return INF;
	return 1/fabs(x);
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

inline static double toPolarD(double x, double y)
{
	return sqrt(x * x + y * y);
}

inline static double toPolarT(double x, double y)
{
	return atan2(y, x);
}

inline static double toCartesianX(double d, double t)
{
	return d * cos(t);
}

static double toCartesianY(double d, double t)
{
	return d * sin(t);
}

}
#endif
