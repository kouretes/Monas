#include "GridMap.h"
#include <iostream>
#include <string.h>


using namespace std;

void GridMap::setupGrid (int radius, int cells, float realRadius) {
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

	cellsOfRadius = radius;
	cellsOfRing = cells;
	radiusInMeters = realRadius;
	//forward step in meters = radius in meters / the number of circles in the polar grid
	moveStepInMeters = radiusInMeters / (float) cellsOfRadius;
	//turn step in meters = 2 * pi / number of cells in each circle
	turnStepInRads = 2 * M_PI / (float) cellsOfRing;
	gridMap = new float*[cellsOfRadius];
	secondaryMap = new float*[cellsOfRadius];
	changed = new int*[cellsOfRadius];
	realX = new float*[cellsOfRadius];
	realY = new float*[cellsOfRadius];

	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		gridMap[ring] = new float[cellsOfRing];
		secondaryMap[ring] =  new float[cellsOfRing];
		changed[ring] =  new int[cellsOfRing];
		realX[ring] = new float[cellsOfRing];
		realY[ring] = new float[cellsOfRing];

		for (int cell = 0; cell < cellsOfRing; cell++) {
			gridMap[ring][cell] = 0;
			realX[ring][cell] = KMath::toCartesianX (ring * moveStepInMeters + moveStepInMeters / 2, cell * turnStepInRads);
			realY[ring][cell] = KMath::toCartesianY (ring * moveStepInMeters + moveStepInMeters / 2, cell * turnStepInRads);
		}
	}

	/*gridMap[2][0] = 1;
	gridMap[2][4] = 0.8;
	gridMap[2][8] = 0.6;
	gridMap[2][12] = 0.4;
	gridMap[2][16] = 0.2;*/
	currentPhiValue = 0.0f;
	currentXValue = 0.0f;
	currentYValue = 0.0f;
}

void GridMap::rotateGrid (float turnStep) {
	currentPhiValue += turnStep;
	bool turnLeft;
	int howMuchToTurn = 0; //0 = No turn, 1 = turn left, 2 = turn right

	while (currentPhiValue <= -turnStepInRads) {
		cout << " asdfasdf " << endl;
		currentPhiValue += turnStepInRads;
		howMuchToTurn++;
		turnLeft = false;
	}

	while (currentPhiValue >= turnStepInRads) {
		cout << " MPika " << currentPhiValue << endl;
		currentPhiValue -= turnStepInRads;
		howMuchToTurn++;
		turnLeft = true;
	}

	cout << howMuchToTurn << " 1) " << turnStep << " 2)" << turnStepInRads << "3) " << currentPhiValue << endl << endl;

	if (howMuchToTurn != 0) {
		turnGrid (turnLeft, howMuchToTurn);
	}
}

void GridMap::turnGrid (bool turnLeft, int times) {
	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		memcpy (secondaryMap[ring], gridMap[ring], cellsOfRing * sizeof (float) );
	}

	if (turnLeft == false) {
		for (int ring = 0; ring < cellsOfRadius; ring++) {
			for (int cell = times; cell < cellsOfRing; cell++) {
				gridMap[ring][cell-times] = secondaryMap[ring][cell];
			}

			for (int cell = 0; cell < times; cell++) {
				gridMap[ring][cell + (cellsOfRing - times) ] = secondaryMap[ring][cell];
			}
		}
	} else {
		for (int ring = 0; ring < cellsOfRadius; ring++) {
			for (int cell = 0; cell < cellsOfRing - times; cell++) {
				gridMap[ring][cell+times] = secondaryMap[ring][cell];
			}

			for (int cell = cellsOfRing - times; cell < cellsOfRing; cell++) {
				gridMap[ring][cell - (cellsOfRing - times) ] = secondaryMap[ring][cell];
			}
		}
	}
}

void GridMap::translateGrid (float x, float y) {
	currentXValue += x;
	currentYValue += y;
	float moveValueX = 0.0f;
	float moveValueY = 0.0f;

	if (fabs (currentXValue) >= moveStepInMeters || fabs (currentYValue) >= moveStepInMeters) {
		cout << moveValueX << " " << moveValueY << endl;

		while (currentXValue > moveStepInMeters) {
			currentXValue -= moveStepInMeters;
			moveValueX += moveStepInMeters;
		}

		cout << moveValueX << " " << moveValueY << endl;

		while (currentXValue < -moveStepInMeters) {
			currentXValue += moveStepInMeters;
			moveValueX -= moveStepInMeters;
		}

		cout << moveValueX << " " << moveValueY << endl;

		while (currentYValue > moveStepInMeters) {
			currentYValue -= moveStepInMeters;
			moveValueY += moveStepInMeters;
		}

		cout << moveValueX << " " << moveValueY << endl;

		while (currentYValue < -moveStepInMeters) {
			currentYValue += moveStepInMeters;
			moveValueY -= moveStepInMeters;
		}

		cout << moveValueX << " " << moveValueY << endl;
		moveGrid (moveValueX, moveValueY);
	}
}

void GridMap::moveGrid (float moveX, float moveY) {
	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		memcpy (secondaryMap[ring], gridMap[ring], cellsOfRing * sizeof (float) );

		for (int cell = 0; cell < cellsOfRing; cell++) {
			gridMap[ring][cell] = 0.0f;
			changed[ring][cell] = 0;
		}
	}

	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			float x = realX[ring][cell];
			float y = realY[ring][cell];
			//cout << "dist = " <<  i*moveStepInMeters + moveStepInMeters/2 << " dir = " << j*turnStepInRads << endl;
			x -= moveX;
			y -= moveY;
			//cout << "x = " <<  x << " y = " << y << endl;
			//cout << "Float dist = " <<  (KMath::toPolarD(x,y)-moveStepInMeters/2)/moveStepInMeters << " float dir = " << (KMath::toPolarT(x,y))/turnStepInRads << endl;
			int dist = round ( (KMath::toPolarD (x, y) - moveStepInMeters / 2) / moveStepInMeters);
			//Wrap to 0 to 2pi before the quantization step
			int dir = round (KMath::wrapTo0_2Pi (KMath::toPolarT (x, y) ) / turnStepInRads);

			if (dist < cellsOfRadius && dist >= 0) {
				gridMap[dist][dir] += secondaryMap[ring][cell];
				changed[dist][dir]++;
			}
		}
	}

	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			if (changed[ring][cell] != 0) {
				gridMap[ring][cell] /= changed[ring][cell];
			}
		}
	}
}

void GridMap::updateGrid (float distance, float sonarAngle, float coneAngle, bool addOnlyTheObject) {
	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			float leftBorder = KMath::wrapTo0_2Pi (sonarAngle + coneAngle / 2);
			float rightBorder = KMath::wrapTo0_2Pi (sonarAngle - coneAngle / 2);
			float cellAngle = cell * turnStepInRads;

			if ( (cellAngle <= leftBorder && cellAngle > rightBorder) || (rightBorder > leftBorder && (cellAngle < leftBorder || cellAngle > rightBorder) ) ) {
				if (!addOnlyTheObject && distance > ring *moveStepInMeters && distance >= (ring + 1) *moveStepInMeters) {
					gridMap[ring][cell] -= 0.1;
				} else if (distance >= ring *moveStepInMeters && distance <= (ring + 1) *moveStepInMeters) {
					if (addOnlyTheObject) {
						gridMap[ring][cell] = 1;
					} else {
						gridMap[ring][cell] += 0.2;
					}
				}

				if (gridMap[ring][cell] > 1) {
					gridMap[ring][cell] = 1;
				} else if (gridMap[ring][cell] < -1) {
					gridMap[ring][cell] = -1;
				}
			}
		}
	}
}

void GridMap::softUpdateGrid (float distance, float sonarAngle, float coneAngle, bool addOnlyTheObject) {
	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			float leftBorder = KMath::wrapTo0_2Pi (sonarAngle + coneAngle / 2);
			float rightBorder = KMath::wrapTo0_2Pi (sonarAngle - coneAngle / 2);
			float cellAngle = cell * turnStepInRads;

			if ( (cellAngle <= leftBorder && cellAngle > rightBorder) || (rightBorder > leftBorder && (cellAngle < leftBorder || cellAngle > rightBorder) ) ) {
				if (!addOnlyTheObject && distance > ring *moveStepInMeters && distance >= (ring + 1) *moveStepInMeters) {
					gridMap[ring][cell] -= 0.05;
				} else if (distance >= ring *moveStepInMeters && distance <= (ring + 1) *moveStepInMeters) {
					if (addOnlyTheObject) {
						gridMap[ring][cell] = 1;
					} else {
						gridMap[ring][cell] += 0.1;
					}
				}

				if (gridMap[ring][cell] > 1) {
					gridMap[ring][cell] = 1;
				} else if (gridMap[ring][cell] < -1) {
					gridMap[ring][cell] = -1;
				}
			}
		}
	}
}

void GridMap::updateCells() {
	for (int ring = 0; ring < cellsOfRadius; ring++ ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			if (gridMap[ring][cell] < -0.02) {
				gridMap[ring][cell] += 0.02;
			} else if (gridMap[ring][cell] > 0.02) {
				gridMap[ring][cell] -= 0.02;
			}

			if (gridMap[ring][cell] > 1) {
				gridMap[ring][cell] = 1;
			} else if (gridMap[ring][cell] < -1) {
				gridMap[ring][cell] = -1;
			}
		}
	}
}

void GridMap::prettyPrint() {
	for (int ring = cellsOfRadius - 1; ring >= 0; ring-- ) {
		for (int cell = 0; cell < cellsOfRing; cell++) {
			cout << gridMap[ring][cell] << " ";
		}
		cout << endl;
	}
}
