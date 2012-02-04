#ifndef MathFunctions_H
#define MathFunctions_H

#include <math.h>
		
		static double wrapToPi(double angle) {
			//			double angle = wrapTo0_2Pi(angle) - M_PI;
			//			if(angle>M_PI)
			//				return angle -2*M_PI;
			//			return angle;
			while (angle > M_PI)
				angle -= 2.0 * M_PI;
			while (angle < -M_PI)
				angle += 2.0 * M_PI;
			return angle;
		}
		static double wrapTo2Pi(double angle) {
			//angle = fmod(M_PI * 2 + angle, M_PI * 2);

			while (angle > 2.0 * M_PI)
				angle -= 2.0 * M_PI;
			while (angle < -2.0 * M_PI)
				angle += 2.0 * M_PI;
			return angle;
		}
		static double wrapTo0_2Pi(double angle) {
			//return fmod(M_PI * 2.0 + angle, M_PI * 2.0);
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
#endif
