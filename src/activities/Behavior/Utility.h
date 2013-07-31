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
	
	const float THRESHOLD_COLLISION = 1.5f;
	const float THRESHOLD_SPARSE = 0.7f;
	const float HIGH_PROB = 1.0f;
	const float LOWER_PROB = 0.4f;
	
	const unsigned int DIM = 2;
	
	/*
	 * Gaussian function parameters.
	 */
	const float A_onBall = 100.0f;
	const float A_prDef = 75.0f;
	const float A_secDef = 65.0f;
	const float A_sup = 65.0f;
	
	// low sigma gives better solutions...!!
	const float sigmaX = 0.2f;
	const float sigmaY = 0.2f;
	
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
	 * rotate to face a position on the field. The absolute angle is divided by PI to become normalized
	 * on [0,1] to represent the cost.
	 * @returns a float value normalized on [0,1].
	 */
	float minRotation(float xRobot, float yRobot, float xPos, float yPos, float thetaRobot) {
		
		float thetaRot = 0.0f, phi;
		
		phi = atan2( (yPos-yRobot), (xPos-xRobot) );
		thetaRot = anglediff2(phi, thetaRobot);
		
		return ( fabs(thetaRot) / M_PI );
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
		
		if(diff <= THRESHOLD_COLLISION)
			return HIGH_PROB;
		else
			return LOWER_PROB;
	}
	
	/**
	 * @fn float collisions(vector<FormationParameters::Role> &mapping, vector<Robot> &robots, FormationGenerator &fGen, unsigned int robotIndex)
	 * @brief Collisions feature that checks for collisions between a given robot destination position and all other robots trajectories on a specific mapping.
	 */
	float collisions(vector<unsigned int> &mapping, vector<Robot> &robots, FormationGenerator &fGen, unsigned int robotIndex) {
        
        float maxCost = 0.0f, cost = 0.0f;
        posInfo otherRobotPos;
        posInfo currentRobotPos = fGen.findRoleInfo(mapping[robotIndex]);
        
        for(unsigned int r = 0 ; r < robots.size() ; r++) {
        	if(r != robotIndex) {
        		otherRobotPos = fGen.findRoleInfo(mapping[r]);
        		cost = checkCollisions(robots[robotIndex].robotX, robots[robotIndex].robotY, robots[r].robotX, robots[r].robotY,
        								currentRobotPos.X, currentRobotPos.Y, otherRobotPos.X, otherRobotPos.Y);
        		if(maxCost < cost)
                   maxCost = cost;
        	}
        }
        
		return maxCost;
	}
	
	/**
	 * @fn float collisionsPSO(vector<float> &x, vector<Robot> &robots, unsigned int robotIndex)
	 * @brief Collisions feature that checks for collisions between a given robot destination position and all other robots trajectories
	 * on a specific particle. Used for PSO algorithm.
	 */
	float collisionsPSO(vector<float> &x, vector<Robot> &robots, unsigned int robotIndex) {
		
		float maxCost = 0.0f, cost = 0.0f;
		
		for(unsigned int r = 0 ; r < DIM*robots.size() ; r += 2) {
        	if(r != robotIndex) {
        		cost = checkCollisions(robots[robotIndex].robotX, robots[robotIndex].robotY, robots[r].robotX, robots[r].robotY,
        		 						x.at(robotIndex), x.at(robotIndex + 1), x.at(r), x.at(r + 1));
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
	float fieldUtility(float posX, float posY, float ballY, FormationGenerator &fGen) {
	   	
	   	float u = 0.0f;
	   	
		for(unsigned int i = 1 ; i < fGen.getFormation()->size() ; i++) { // exclude goalkeeper
			if(fGen.getFormation()->at(i).role == ONBALL)
				u = u + gaussian2D(A_onBall, fGen.getFormation()->at(i).X, fGen.getFormation()->at(i).Y, sigmaX, sigmaY, posX, posY);
			else if(fGen.getFormation()->at(i).role == SUPPORTER || fGen.getFormation()->at(i).role == SUPPORTER_L || fGen.getFormation()->at(i).role == SUPPORTER_R)
				u = u + gaussian2D(A_sup, fGen.getFormation()->at(i).X, fGen.getFormation()->at(i).Y, sigmaX, sigmaY, posX, posY);
			else if(fGen.getFormationType() == OFFENSIVE)
				u = u + gaussian2D(A_secDef, fGen.getFormation()->at(i).X, fGen.getFormation()->at(i).Y, sigmaX, sigmaY, posX, posY);
			else if(fGen.getFormationType() == DEFENSIVE) {
				if( (ballY <= 0 && fGen.getFormation()->at(i).Y <= 0) || (ballY > 0 && fGen.getFormation()->at(i).Y > 0) )
					u = u + gaussian2D(A_prDef, fGen.getFormation()->at(i).X, fGen.getFormation()->at(i).Y, sigmaX, sigmaY, posX, posY);
				else
					u = u + gaussian2D(A_secDef, fGen.getFormation()->at(i).X, fGen.getFormation()->at(i).Y, sigmaX, sigmaY, posX, posY);
			}
		}
		
		return u;
    }
	
	/**
	 * @fn float sparse(vector<unsigned int> &mapping, unsigned int numOfRobots, FormationGenerator &fGen, unsigned int robotIndex)
	 * @brief Use to determine the sparse cost of the robots on the field.
	 */
    float sparse(vector<unsigned int> &mapping, unsigned int numOfRobots, FormationGenerator &fGen, unsigned int robotIndex) {
    
    	float maxCost = 0.0f, dist = 0.0f;
        posInfo otherRobotPos;
        posInfo currentRobotPos = fGen.findRoleInfo(mapping[robotIndex]);
        
        for(unsigned int r = 0 ; r < numOfRobots ; r++) {
        	if(r != robotIndex) {
        		otherRobotPos = fGen.findRoleInfo(mapping[r]);
        		dist = DISTANCE(currentRobotPos.X, otherRobotPos.X, currentRobotPos.Y, otherRobotPos.Y);
        		if(dist <= THRESHOLD_SPARSE)
                   maxCost = maxCost + 100;
        	}
        }
        
		return maxCost;
    }
    
    /**
	 * @fn float sparsePSO(vector<float> &x, unsigned int robotIndex, unsigned int numOfRobots)
	 * @brief Feature to determine the sparse cost of the robots on the field for a specific particle. Used for
	 * PSO algorithm.
	 */
    float sparsePSO(vector<float> &x, unsigned int robotIndex, unsigned int numOfRobots) {
    	float maxCost = 0.0f, dist = 0.0f;
    	
    	for(unsigned int r = 0 ; r < 2*numOfRobots ; r += 2) {
        	if(r != robotIndex) {
        		dist =  DISTANCE(x.at(robotIndex), x.at(r), x.at(robotIndex + 1), x.at(r + 1));
        		
        		if(dist <= THRESHOLD_SPARSE)
                   maxCost = maxCost + 100;
        	}
        }
        
		return maxCost;
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
	
	/**
     * @fn float U(vector<float> &x, float ballY, FormationGenerator &fGen, vector<Robot> &robots)
     * @brief Used by the PSO algorithm to evaluate a specific particle. This function we try to maximize
     * during the iterations of the PSO algorithm.
     */
	float U(vector<float> &x, float ballY, FormationGenerator &fGen, vector<Robot> &robots) {
		
		float ul = 0.0f;
		unsigned int cnt = 0;
		
		for(unsigned int robotIndex = 0 ; robotIndex < robots.size() ; robotIndex++) {
			ul = ul + fieldUtility(x.at(cnt), x.at(cnt + 1), ballY, fGen)
					- distance(robots.at(robotIndex).robotX, robots.at(robotIndex).robotY, x.at(cnt), x.at(cnt + 1), fGen.Field.MaxX, fGen.Field.MaxY)
					- minRotation(robots.at(robotIndex).robotX, robots.at(robotIndex).robotY, x.at(cnt), x.at(cnt + 1), robots.at(robotIndex).robotPhi)
					- sparsePSO(x, cnt, robots.size())
					- collisionsPSO(x, robots, cnt);
			cnt += 2;
		}
		
		return ul;
	}
		
};

#endif
