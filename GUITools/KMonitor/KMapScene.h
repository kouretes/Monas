#ifndef KMAPSCENE_H_
#define KMAPSCENE_H_

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

#define ImgScale	400
#define ColorMax 	255
#define ArrowOffset	10

#define ToDegrees 	(180.0/M_PI)
#define ToRad 		(M_PI/180.0)

#define M 			10
#define N 			18
#define TotalRings	(1+M+1)
#define InnerRing	(0)
#define OuterRing	(1+M)
#define NEIGHBOURS 	8

#define RobotRadius 	0.2
#define MapRadius 		0.63  // prev Value 1.0
#define ObstacleRadius 	0.15
#define RingDistance 	( (MapRadius) / (M) )
#define SectorAngleDeg 	(360.0/N) // deg
#define SectorAngleRad 	(SectorAngleDeg*ToRad)
#define SectorShiftRad	( ( (N%4) == 0 ) ? 0.5*SectorAngleRad : 0.0 )

#define FRONT 			(int(N/4))
#define SIDE_LEFT 		(int(N/2))
#define SIDE_RIGHT 		0

#define SonarAngleShiftDeg	20.0 // deg
#define SonarAngleShiftRad	(SonarAngleShiftDeg*ToRad)
#define SonarDistanceShift	0.065
#define SonarAngleRangeDeg	45.0 // deg
#define SonarAngleRangeRad 	(SonarAngleRangeDeg*ToRad)
#define SonarDistanceRange 	1.0
#define SonarMinReading		0.4
#define SonarReadingsNum 	10

#define NoKnowledge 			0.5
#define AgeFactor 				0.8
#define OccupancyDecreaseFactor 0.8
#define OccupancyIncreaseFactor	1.5
#define MinOccupancy 			0.08
#define MaxOccupancy 			1.0

#define PathLength 			50
#define ObstacleThreshold	0.75
#define ObstacleCostFactor	500.0
#define RotationCostFactor	3.5
#define SideStepCostFactor	5.0
#define SemiSideCostFactor	4.0

#include <QGraphicsScene>
#include <QList>
#include "KMapView.h"

class KMapView;

class KMapScene : public QGraphicsScene
{

public:
	KMapScene(KMapView* parent, QString hostId);
	~KMapScene();

	void resizeMapScene(int size);
	QString getCurrentHost(){ return currentHost; }

	void setLPMObstaclesVisible(bool visible){ LPMObstaclesVisible = visible; setPMObstaclesVisible(visible);}
	bool getLPMObstaclesVisible(){return LPMObstaclesVisible;}
	void setPMObstaclesVisible(bool visible);
	void updateObstacles(bool initialization);
	void updateArrow();

	void setLPMTargetCoordVisible(bool visible){ LPMTargetCoordVisible = visible; setPMTargetCoordVisible(visible);}
	bool getLPMTargetCoordVisible(){return LPMTargetCoordVisible;}
	void setPMTargetCoordVisible(bool visible);
	void updateTargetCoordinates();

	void setLPMPathVisible(bool visible){ LPMPathVisible = visible; setPMPathVisible(visible);}
	bool getLPMPathVisible(){return LPMPathVisible;}
	void setPMPathVisible(bool visible);
	void updatePath();

	double targetX, targetY, targetA;
	int pathR[PathLength], pathS[PathLength], pathO[PathLength];
	double PolarGrid[2][TotalRings][N];

private:
	void initGrid();
	void initCoordinates();
	void pathLineListRectReset();

	KMapView* parent;
	QString currentHost;

	QList<QGraphicsPolygonItem*> cellsInit;

	bool LPMObstaclesVisible;
	QList<QGraphicsPolygonItem*> cellsList;

	QGraphicsLineItem* arrowBody;
	QGraphicsLineItem* arrowLside;
	QGraphicsLineItem* arrowRside;

	bool LPMTargetCoordVisible;
	QGraphicsEllipseItem* targetBall;
	QGraphicsLineItem* targetLine;

	bool LPMPathVisible;
	QList<QGraphicsLineItem*> pathLineList;


	// Math functions headers
	double wrapToPi(double angle);
	double wrapTo0_2Pi(double angle);
	double angleDiff(double a1, double a2);
	double toPolarD(double x, double y);
	double toPolarT(double x, double y);
	double toCartesianX(double d, double t);
	double toCartesianY(double d, double t);
	int DtoR(double d);
	int TtoS(double theta);
	double RtoD(int r);
	double StoT(int s);
	int XYtoR(double x, double y);
	int XYtoS(double x, double y);
	int toGrid(double x);
	int wrapTo(int n, int MAXN);

	//Iris variables
	struct OpenListNode {
		int ring;
		int sector;
		int orientation;
		double angle;
		double gn;
		double hn;
		double fn;
	};


	double cellCenterX[TotalRings][N],
		   cellCenterY[TotalRings][N];
	double euclidean[TotalRings][N][TotalRings][N];

	int gridImgH[TotalRings+1][N],
		gridImgV[TotalRings+1][N];

	int present ;
	int ImgSize;
	int ImgShift;

	int shiftGui;
	int x[TotalRings+1][N], y[TotalRings+1][N];



};

#endif /* KMAPSCENE_H_ */
