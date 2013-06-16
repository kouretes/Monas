#ifndef MATHCOMMON_H
#define MATHCOMMON_H

#include <math.h>
#include <limits>

// DO NOT USE DEFINES, USE THE BLOODY FUNCTIONS !!!

#define TO_RAD_SUPER_INTERNAL_DO_NOT_USE 0.01745329

namespace KMath {

	const float INF = std::numeric_limits<float>::infinity();

	inline static double TO_RAD(double X) { return X * TO_RAD_SUPER_INTERNAL_DO_NOT_USE; }
	inline static double TO_DEG(double X) { return X / TO_RAD_SUPER_INTERNAL_DO_NOT_USE; }

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
	 * @fn inline static unsigned int factorial(unsigned int x)
	 * @brief The product of all positive integers less than or equal to x. Recursive calculation.
	 */
	inline static unsigned int factorial(unsigned int x) {
		return (x == 1 ? x : x * factorial(x - 1));
	}

	/**
	 * @fn inline static unsigned int binomialCoefficient(unsigned int n, unsigned int k)
	 * @brief For any set containing n elements, the number of distinct k-element subsets of it that can be formed
	 * (the k-combinations of its elements) is given by the binomial coefficient. Calculated using factorials
	 * formula.
	 */
	inline static unsigned int binomialCoefficient(unsigned int n, unsigned int k) {
		return (factorial(n) / (factorial(k)*factorial(n-k)) );
	}

	/**
	 * @fn inline static float gaussian2D(float A, float xCenter, float yCenter, float sigmaX, float sigmaY, float x, float y)
	 * @brief 2-Dimensional Gaussian function.
	 * @parameter A is the amplitude.
	 * @parameter xCenter, yCenter is the center of the blob.
	 * @parameter sigmaX, sigmaY are the x and y spreads of the blob.
	 */
	inline static float gaussian2D(float A, float xCenter, float yCenter, float sigmaX, float sigmaY, float x, float y) {
		return A*exp( -( pow(x - xCenter, 2)/(2*pow(sigmaX, 2)) + pow(y - yCenter, 2)/(2*pow(sigmaY, 2)) ) );
	}

	/**
	 * @fn template<typename T> inline T FUNC1ByX(T x)
	 * @brief function f(x)=1/|x| used to generate supporters positions.
	 * @return the result of 1/|x| or infinity number in case x = 0.
	 */
	template<typename T> inline T func1ByAbsX(T x) {
		if (x == 0)
			return INF;
		return 1/fabs(x);
	}

	inline static double wrapToPi(double angle) {
		while (angle > M_PI)
			angle -= 2.0 * M_PI;

		while (angle < -M_PI)
			angle += 2.0 * M_PI;

		return angle;
	}

	inline static double wrapTo2Pi(double angle) {
		while (angle > 2.0 * M_PI)
			angle -= 2.0 * M_PI;

		while (angle < -2.0 * M_PI)
			angle += 2.0 * M_PI;

		return angle;
	}

	inline static double wrapTo0_2Pi(double angle) {
		while (angle > 2.0 * M_PI)
			angle -= 2.0 * M_PI;

		while (angle < 0)
			angle += 2.0 * M_PI;

		return angle;
	}

	inline static double anglediff2(double a1, double a2) {
		return wrapToPi(wrapToPi(a1 + M_PI - a2) - M_PI);
	}

	inline static double anglediff(double a1, double a2) {
		return fabs(wrapTo0_2Pi(a1 + M_PI - a2) - M_PI);
	}

	inline static double toPolarD(double x, double y) {
		return sqrt(x * x + y * y);
	}

	inline static double toPolarT(double x, double y) {
		return atan2(y, x);
	}

	inline static double toCartesianX(double d, double t) {
		return d * cos(t);
	}

	static double toCartesianY(double d, double t) {
		return d * sin(t);
	}

}
#endif
