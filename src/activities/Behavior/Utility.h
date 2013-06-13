#ifndef UTILITY_H
#define UTILITY_H

#include "core/elements/math/Common.hpp"
#include "FormationParameters.h"

using namespace KMath;
using namespace FormationParameters;

/**
 * Namespace that contains all the utility/cost function features for coordination
 * algorithm. Any future features for this function should be included here!
 */
namespace Utility {
	
	const float THRESHOLD = 1.5f;
	const float HIGH_PROB = 1.0f;
	const float LOWER_PROB = 0.4f;

	/*
	 * Gaussian function parameters.
	 */
	const float A_onBall = 100.0f;
	const float A_prDef = 65.0f;
	const float A_secDef = 45.0f;
	const float sigmaX = 0.5f;
	const float sigmaY = 0.5f;
	
	/*
	 * Specific roles information.
	 */
	posInfo onBall, defenderC, defenderL, defenderR;
	
	/**
	 * @fn float distance(xRobot, yRobot, xPos, yPos)
	 * @brief Distance cost feature that calculates the distance between robot position and
	 * a position on the field. The distance is normalized on [0,1] to represent the cost.
	 * @returns a float value normalized on [0,1].
	 */
	float distance(float xRobot, float yRobot, float xPos, float yPos, float MaxX, float MaxY) {		
 		return sqrt( pow(xRobot-xPos, 2) + pow(yRobot-yPos, 2) ) / sqrt( pow(2*MaxX, 2) + pow(2*MaxY, 2) );
	}
	
	/**
	 * @fn float minRotation(xRobot, yRobot, xPos, yPos, thetaRobot)
	 * @brief Min rotation cost feature that calculates the minimum angle that robot has to
	 * rotate to face a position on the field. The angle is divided by PI to become normalized
	 * on [0,1] to represent the cost.
	 * @returns a float value normalized on [0,1].
	 */
	float minRotation(float xRobot, float yRobot, float xPos, float yPos, float thetaRobot) {
		
		float thetaRot = 0.0f, phi;

		phi = atan( (yPos-yRobot)/(xPos-xRobot) );
		
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
				return Min(thetaRot, (float)fabs(2*M_PI - thetaRot))/M_PI;
			}
		}
		else if(xPos > xRobot && yPos < yRobot) {
			phi = M_PI + phi;
			if(thetaRobot <= M_PI) {
				return (M_PI - fabs(phi - thetaRobot))/M_PI;			
			}
			else {
				thetaRot = M_PI - fabs(phi - thetaRobot);
				return Min(thetaRot, (float)fabs(2*M_PI - thetaRot))/M_PI;			
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
				return Min(thetaRot, (float)fabs(2*M_PI - thetaRot))/M_PI;
			}
		}
		else if(xPos < xRobot && yPos < yRobot) {
			if(thetaRobot <= M_PI) {
				return (M_PI - fabs(phi - thetaRobot))/M_PI;		
			}		
			else {
				thetaRot = M_PI - fabs(phi - thetaRobot);
				return Min(thetaRot, (float)fabs(2*M_PI - thetaRot))/M_PI;
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
	float checkCollisions(float xRobot1, float yRobot1, float xRobot2, float yRobot2, float xPos1, float yPos1, float xPos2, float yPos2) {
		
		float a1, a2, b1, b2, Px, Py, distanceRobot1, distanceRobot2, diff;
		
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
		
		distanceRobot1 = DISTANCE(xRobot1, Px, yRobot1, Py);
		distanceRobot2 = DISTANCE(xRobot2, Px, yRobot2, Py);
		
		diff = fabs(distanceRobot1 - distanceRobot2);
		
		if(diff <= THRESHOLD)
			return HIGH_PROB;
		else
			return LOWER_PROB;
	}
	
	/**
	 * @fn float collisions(vector<FormationParameters::Role> &mapping, vector<Robot> &robots, FormationGenerator &fGen, unsigned int robotIndex, float robotX, float robotY)
	 * @brief Collisions feature that checks for collisions between a given robot destination position and all other robots trajectories on a specific mapping.
	 * TODO maybe we can make it simpler!!!
	 */
	float collisions(vector<FormationParameters::Role> &mapping, vector<Robot> &robots, FormationGenerator &fGen, unsigned int robotIndex, float robotX, float robotY) {
        
        float maxCost = 0.0f, cost = 0.0f;
        posInfo otherRobotPos;
        posInfo currentRobotPos = fGen.findRoleInfo(mapping[robotIndex]);
        
        for(unsigned int r = 0 ; r < robots.size() ; r++) {
        	if(r != robotIndex) {
        		otherRobotPos = fGen.findRoleInfo(mapping[r]);
        		cost = checkCollisions(robotX, robotY, robots[r].robotX, robots[r].robotY, currentRobotPos.X, currentRobotPos.Y, otherRobotPos.X, otherRobotPos.Y);
        		
        		if(maxCost < cost)
                   maxCost = cost;
        	}
        }
        
		return maxCost;
	}
	
	/**
	 * @fn float fieldUtility(float posX, float posY, float ballY, FormationGenerator &fGen, Type formationType)
	 * @brief Field feature that calculates a value for a given position on the field and the formation type. The feature
	 * is based on 2D gaussian functions that favors certain positions on the field.
	 */
	float fieldUtility(float posX, float posY, float ballY, FormationGenerator &fGen, Type formationType) {
	   
		if(formationType == OFFENSIVE) {
			onBall = fGen.findRoleInfo(ONBALL);
			defenderC = fGen.findRoleInfo(DEFENDER);
			return gaussian2D(A_onBall, onBall.X, onBall.Y, sigmaX, sigmaY, posX, posY) + gaussian2D(A_prDef, defenderC.X, defenderC.Y, sigmaX, sigmaY, posX, posY);
		}
		else if(formationType == DEFENSIVE) {
			onBall = fGen.findRoleInfo(ONBALL);
			defenderL = fGen.findRoleInfo(DEFENDER_L);
			defenderR = fGen.findRoleInfo(DEFENDER_R);
			if(ballY <= 0) {
				return gaussian2D(A_onBall, onBall.X, onBall.Y, sigmaX, sigmaY, posX, posY) + gaussian2D(A_prDef, defenderR.X, defenderR.Y, sigmaX, sigmaY, posX, posY) +
						gaussian2D(A_secDef, defenderL.X, defenderL.Y, sigmaX, sigmaY, posX, posY);
			}
			else {
				return gaussian2D(A_onBall, onBall.X, onBall.Y, sigmaX, sigmaY, posX, posY) + gaussian2D(A_prDef, defenderL.X, defenderL.Y, sigmaX, sigmaY, posX, posY) +
						gaussian2D(A_secDef, defenderR.X, defenderR.Y, sigmaX, sigmaY, posX, posY);
			}
		}
    }
    
    /**
     * @fn float robotStability(unsigned int cnt)
     * @brief Robot stability feature that calculates a cost for robot, depending
     * on a number representing how many times the robot has fallen and has be penalized.
     */
    float robotStability(unsigned int cnt) {
    	if(cnt <= 9)
			return (cnt * 0.1);
		else
			return 1.0f;
	}
	
};

#endif
