#ifndef UTILITY_H
#define UTILITY_H

#include "core/elements/math/Common.hpp"
#include "FormationGenerator.h"

using namespace FormationConsts;

namespace Utility {
	
	const float THRESHOLD = 1.5f;
	const float HIGH_PROB = 1.0f;
	const float LOWER_PROB = 0.4f;

	/*
	 * Gaussian function parameters.
	 */
	float A1 = 100.0f, A2 = 65.0f;
	float sigmaX = 0.5f, sigmaY = 0.5f;
	float xCenter1, yCenter1, xCenter2, yCenter2;
	
	/**
	 * @fn float distance(xRobot, yRobot, xPos, yPos)
	 * @brief Distance cost feature that calculates the distance between robot position and
	 * a position on the field. The distance is normalized on [0,1] to represent the cost.
	 * @returns a float value normalized on [0,1].
	 */
	float distance(xRobot, yRobot, xPos, yPos) {		
 		return sqrt( pow(xRobot-xPos, 2) + pow(yRobot-yPos, 2) ) / sqrt( pow(2*Field.MaxX, 2) + pow(2*Field.MaxY, 2) );
	}
	
	/**
	 * @fn float minRotation(xRobot, yRobot, xPos, yPos, thetaRobot)
	 * @brief Min rotation cost feature that calculates the minimum angle that robot has to
	 * rotate to face a position on the field. The angle is divided by PI to become normalized
	 * on [0,1] to represent the cost.
	 * @returns a float value normalized on [0,1].
	 */
	float minRotation(xRobot, yRobot, xPos, yPos, thetaRobot) {
		
		float thetaRot = 0.0f;

		phi = atan( (yPos-yRobot)\(xPos-xRobot) );
		
		if(xPos <= xRobot && yPos >= yRobot) {
			if(xPos == xRobot && yPos == yRobot) {
				return 0.0f;				
			}
			phi = M_PI + phi;
			if(thetaRobot <= M_PI) {
				return fabs(phi - thetaRobot)/M_PI;			
			}
			else {
				thetaRot = fabs(phi - thetaRobot);
				return Min(thetaRot, fabs(2*M_PI - thetaRot))/M_PI;
			}
		}
		else if(xPos > xRobot && yPos < yRobot) {
			phi = M_PI + phi;
			if(thetaRobot <= M_PI) {
				return (M_PI - fabs(phi - thetaRobot))/M_PI;			
			}
			else {
				thetaRot = M_PI - fabs(phi - thetaRobot);
				return Min(thetaRot, fabs(2*M_PI - thetaRot))/M_PI;			
			}
		}
		else if(xPos >= xRobot && yPos >= yRobot) {
			if(xPos == xRobot && yPos == yRobot) {
				return 0.0f;			
			}
			if(thetaRobot <= M_PI) {
				return fabs(phi - thetaRobot)/M_PI;			
			}
			else {
				thetaRot = fabs(phi - thetaRobot);
				return Min(thetaRot, fabs(2*M_PI - thetaRot))/M_PI;
			}
		}
		else if(xPos < xRobot && yPos < yRobot) {
			if(thetaRobot <= M_PI) {
				return (M_PI - fabs(phi - thetaRobot))/M_PI;		
			}		
			else {
				thetaRot = M_PI - fabs(phi - thetaRobot);
				return Min(thetaRot, fabs(2*M_PI - thetaRot))/M_PI;
			}
		}
	}
	
	/**
	 * @fn float checkCollisions(xRobot1, yRobot1, xRobot2, yRobot2, xPos1, yPos1, xPos2, yPos2)
	 * @brief Collision cost feature that calculates the probability of collision given 2 robots
	 * and 2 positions on the field, one for each robot. Look for an intercection on the 2 robots
	 * trajectories and then calculate the distance of each robot to this point on the field if any.
	 * If the difference of the distances is lower than a threshold then the probability is high, else
	 * is lower.
	 * @returns a float value on [0,1].
	 */
	float checkCollisions(xRobot1, yRobot1, xRobot2, yRobot2, xPos1, yPos1, xPos2, yPos2) {
		float a1, a2, b1, b2;
		
		// for the first trajectory (line).
		a1 = (yRobot1 - yPos1)/(xRobot1 - xPos1);
		b1 = yPos1 - a1*xPos1;
		
		// for the second trajectory (line).
		a2 = (yRobot2 - yPos2)/(xRobot2 - xPos2);
		b2 = yPos2 - a2*xPos2;

		Px = (b2 - b1)/(a1 - a2);
		Py = a1*Px + b1;

		if(Min(xRobot1, xPos1) > Px || Max(xRobot1, xPos1) < Px) { // if the intercection is outside the field then ignore it!
			return 0;		
		}
		
		distanceRobot1 = distance(xRobot1, yRobot1, Px, Py);
		distanceRobot2 = distance(xRobot2, yRobot2, Px, Py);
		
		diff = fabs(distanceRobot1 - distanceRobot2);
		
		if(diff <= THRESHOLD)
			return HIGH_PROB;
		else
			return LOWER_PROB;
	}

};

#endif UTILITY_H
