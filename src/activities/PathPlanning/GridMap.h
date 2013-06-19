#ifndef GRIDMAP_H
#define GRIDMAP_H
#include <math.h>
#include <iostream>
#include "core/elements/math/Common.hpp"

/**********************
[x+1,y+1] [x+1,y] [x+1,y-1]
[x  ,y+1] center  [x  ,y-1]
[x-1,y+1] [x-1,y] [x-1,y-1]
**********************/
//Positive are going left

class GridMap {

public:
	GridMap() {
		gridMap = NULL;
		secondaryMap = NULL;
		changed = NULL;
	}

	~GridMap() {
		if (gridMap != NULL) {
			for (int ring = 0; ring < cellsOfRadius; ring++) {
				delete gridMap[ring];
				delete secondaryMap[ring];
				delete changed[ring];
				delete realX[ring];
				delete realY[ring];
			}

			delete gridMap;
			delete secondaryMap;
			delete changed;
			delete realX;
			delete realY;
		}
	}

	enum neighboringCells {UP, UPRIGHT, RIGHT, DOWNRIGHT, DOWN, DOWNLEFT, LEFT, UPLEFT};

	void setupGrid (int radius, int cells, float realRadius);

	float operator() (unsigned int ring, unsigned int cell) const {
		if (gridMap != NULL) {
			return gridMap[ring][cell];
		} else {
			return 0;
		}
	}

	//Rotate grid by turnStep rads (if it is possible)
	void rotateGrid (float turnStep);
	void translateGrid (float x, float y);
	void prettyPrint();
	void moveGrid (float moveX, float moveY);
	void updateGrid (float distance, float sonarAngle, float coneAngle, bool addOnlyTheObject);
	void softUpdateGrid (float distance, float sonarAngle, float coneAngle, bool addOnlyTheObject);
	void updateCells();
	int getRadiusCells(){
		return cellsOfRadius;
	}

	int getRingCells(){
		return cellsOfRing;
	}
	
	float getRealMetters(){
		return radiusInMeters;
	}

	float getMoveSteps(){
		return moveStepInMeters;
	}

	float getTurnsteps(){
		return turnStepInRads;
	}


private:

	//Rotate the grid (left if turnLeft is true) by x (times) steps
	void turnGrid (bool turnLeft, int times);

	//Cells in the line of the radius of the grid (the number of rings)
	int cellsOfRadius;

	//Cells in rings of the grid
	int cellsOfRing;

	//The radius of the grid in real messurements
	float radiusInMeters;

	//How much the robot must move to translate the grid
	float moveStepInMeters;

	//How much the robot must turn to rotate the grid
	float turnStepInRads;

	//The target for the path planning
	int targetPosition[2]; //radius and cell in circuit

	//The grid map
	float **gridMap;
	float **secondaryMap;
	float **realX;
	float **realY;
	int **changed;

	//Store how much have the robot turned until now
	float currentPhiValue;

	//Store how much have the robot moved until now
	float currentXValue;
	float currentYValue;

public:

	float getRealX (int ring, int cell) {
		if (ring > cellsOfRadius || cell > cellsOfRing) {
			return 0.0f;
		} else {
			return realX[ring][cell];
		}
	}

	float getRealY (int ring, int cell) {
		if (ring > cellsOfRadius || cell > cellsOfRing) {
			return 0.0f;
		} else {
			return realY[ring][cell];
		}
	}
};

#endif
