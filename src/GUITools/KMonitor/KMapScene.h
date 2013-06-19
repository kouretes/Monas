#ifndef KMAPSCENE_H_
#define KMAPSCENE_H_

#include <QGraphicsScene>
#include <iomanip>
#include <string>
#include <cstring>
#include <fstream>
#include <math.h>
#include <vector>
#include <map>
#include <string>
#include <cstdlib>
#include <list>

#include "core/elements/math/Common.hpp"

#define ImgScale	400
#define ColorMax 	255
#define ArrowOffset	10

#include <QGraphicsScene>
#include <QList>

class KMapScene : public QGraphicsScene {

public:
	KMapScene(QGraphicsView *parent);
	~KMapScene();

	void resizeMapScene(int size);
	void resetKMapScene();

	void setLPMObstaclesVisible(bool visible) {
		LPMObstaclesVisible = visible;
		setPMObstaclesVisible(visible);
	}
	
	bool getLPMObstaclesVisible() {
		return LPMObstaclesVisible;
	}
	
	void setPMObstaclesVisible(bool visible);
	void updateObstacles(bool initialization);
	void updateArrow();

	void setLPMTargetCoordVisible(bool visible) {
		LPMTargetCoordVisible = visible;
		setPMTargetCoordVisible(visible);
	}
	
	bool getLPMTargetCoordVisible() {
		return LPMTargetCoordVisible;
	}
	
	void setPMTargetCoordVisible(bool visible);
	void updateTargetCoordinates();

	void setLPMPathVisible(bool visible) {
		LPMPathVisible = visible;
		setPMPathVisible(visible);
	}
	
	bool getLPMPathVisible() {
		return LPMPathVisible;
	}
	
	void setPMPathVisible(bool visible);
	void updatePath();

	int targetRing, targetCell;
	float targetOrient;
	int *pathR, *pathS, *pathO;
	int *pathR2, *pathS2, *pathO2;
	int totalVisits;
	
	float **PolarGrid;

	void setupGrid(int cellsRad, int cellsRing, float gridLength, int pathSize);

	int cellsOfRadius;
	int cellsOfRing;
	float moveStepInMeters;
	float turnStepInRads;
	int pathLength;
private:
	void initGrid();
	void initCoordinates();
	void pathLineListRectReset();

	QGraphicsView *parent;


	bool LPMObstaclesVisible;
	QList<QGraphicsPolygonItem*> cellsList;

	QGraphicsLineItem *arrowBody;
	QGraphicsLineItem *arrowLside;
	QGraphicsLineItem *arrowRside;

	bool LPMTargetCoordVisible;
	QGraphicsEllipseItem *targetBall;
	QGraphicsLineItem *targetLine;

	bool LPMPathVisible;

	QList<QGraphicsLineItem *> pathLineList;
	QList<QGraphicsEllipseItem *> pathEllipseList;
	QList<QGraphicsLineItem *> pathSmallLineList;


	QList<QGraphicsEllipseItem *> visitedEllipseList;
	QList<QGraphicsLineItem *> visitedSmallLineList;

	float ringToDistance (int r);
	float cellToTheta (int s);
	int toGrid (float x);
	int wrapTo (int n, int MAXN);

	struct OpenListNode {
		int ring;
		int sector;
		int orientation;
		float angle;
		float gn;
		float hn;
		float fn;
	};

	float **cellCenterX, **cellCenterY;

	int **gridImgH, **gridImgV;

	int ImgSize;
	int ImgShift;

};

#endif // KMAPSCENE_H_
